#include	"..\..\interface.h"
#include	"s_FME7.h"

// Sunsoft FME-7, based on the AY-8910

#define	NES_INC_SIZE_INT (204800000 / 77)

//#define	FME7_NOISE

typedef	struct	FME7sqr
{
	union
	{
		struct
		{
			unsigned freq    :12;
			unsigned         : 4;
			unsigned volume  : 4;
			unsigned envmode : 1;
			unsigned         : 3;
		};
		struct
		{
			unsigned byte0 : 8;
			unsigned byte1 : 8;
			unsigned byte2 : 8;
		};
	};
	u8 CurP;
	s32 LCtr;
}	TFME7sqr, *PFME7sqr;

#ifdef	FME7_NOISE
typedef	struct	FME7noise
{
	union
	{
		struct
		{
			unsigned freq  : 5;
			unsigned       : 3;
		};
		struct
		{
			unsigned byte0 : 8;
		};
	};
	u16_n ShiftReg;
	s32 LCtr;
}	TFME7noise, *PFME7noise;
#endif

typedef	struct	FME7sound
{
	union
	{
		struct
		{
			unsigned tone      : 3;
#ifdef	FME7_NOISE
			unsigned noise     : 3;
#else
			unsigned           : 3;
#endif
			unsigned           : 2;
			unsigned envelope  :16;
			unsigned envhold   : 1;
			unsigned envaltr   : 1;
			unsigned envattk   : 1;
			unsigned envcont   : 1;
			unsigned           : 4;
		};
		struct
		{
			unsigned byte7 : 8;
			unsigned byteB : 8;
			unsigned byteC : 8;
			unsigned byteD : 8;
		};
	};
	u8 select;
	TFME7sqr Sqr[3];
#ifdef	FME7_NOISE
	TFME7noise Noise;
#endif
}	TFME7sound, *PFME7sound;

static	TFME7sound	FME7sound;

static	int	FME7_DoSquare (PFME7sqr ChanData, int Cycles)
{
	ChanData->LCtr -= Cycles;
	while (ChanData->LCtr <= 0)
	{
		ChanData->CurP++;
		ChanData->CurP &= 0xF;
		ChanData->LCtr += (ChanData->freq << 1) + 1;
	}
	return ChanData->volume * ((ChanData->CurP < 8) ? 3 : -3);
}
#ifdef	FME7_NOISE
static	int	FME7_DoNoise (PFME7noise ChanData, int Cycles)
{
	int x, volume = 0;
        for (x = 0; x < 3; x++)
	{
		if (!(FME7sound.noise & (1 << x)))
			volume += FME7sound.Sqr[x].volume;	// 1 noise for each channel
	}
	ChanData->LCtr -= Cycles;
	while (ChanData->LCtr <= -)
	{
		ChanData->ShiftReg.s0 = (ChanData->ShiftReg.s0 << 1) | ((ChanData->ShiftReg.s0 >> 13) & 1) ^ ((ChanData->ShiftReg.s0 >> 14) & 1);
		ChanData->LCtr += (ChanData->freq << 1) + 1;
	}
	return volume * ((ChanData->ShiftReg.s0 & 1) ? 3 : -3);
}
#endif

void	FME7sound_Load (void)
{
	memset(&FME7sound,0,sizeof(FME7sound));
}

void	FME7sound_Reset (RESET_TYPE ResetType)
{
	FME7sound.Sqr[0].LCtr = 1;
	FME7sound.Sqr[1].LCtr = 1;
	FME7sound.Sqr[2].LCtr = 1;
#ifdef	FME7_NOISE
	FME7sound.Noise.ShiftReg.s0 = 0xFFFF;
	FME7sound.Noise.LCtr = 1;
#endif
}

void	FME7sound_Unload (void)
{
}

void	FME7sound_Write (int Addr, int Val)
{
	switch (Addr & 0xE000)
	{
	case 0xC000:	FME7sound.select = Val & 0xF;	break;	
	case 0xE000:	switch (FME7sound.select)
			{
			case 0x0:	FME7sound.Sqr[0].byte0 = Val;	break;
			case 0x1:	FME7sound.Sqr[0].byte1 = Val;	break;
			case 0x2:	FME7sound.Sqr[1].byte0 = Val;	break;
			case 0x3:	FME7sound.Sqr[1].byte1 = Val;	break;
			case 0x4:	FME7sound.Sqr[2].byte0 = Val;	break;
			case 0x5:	FME7sound.Sqr[2].byte1 = Val;	break;
#ifdef	FME7_NOISE
			case 0x6:	FME7sound.Noise.byte0 = Val;	break;
#endif
			case 0x7:	FME7sound.byte7 = Val;	break;
			case 0x8:	FME7sound.Sqr[0].byte2 = Val;	break;
			case 0x9:	FME7sound.Sqr[1].byte2 = Val;	break;
			case 0xA:	FME7sound.Sqr[2].byte2 = Val;	break;
			case 0xB:	FME7sound.byteB = Val;	break;
			case 0xC:	FME7sound.byteC = Val;	break;
			case 0xD:	FME7sound.byteD = Val;	break;
			}				break;
	}
}

int	_MAPINT	FME7sound_Get (int Cycles)
{
	int z = 0;
	if (!(FME7sound.tone & 1))	z += FME7_DoSquare(&FME7sound.Sqr[0],Cycles);
	if (!(FME7sound.tone & 2))	z += FME7_DoSquare(&FME7sound.Sqr[1],Cycles);
	if (!(FME7sound.tone & 4))	z += FME7_DoSquare(&FME7sound.Sqr[2],Cycles);
#ifdef	FME7_NOISE
	if (FME7sound.noise ^ 0x7)	z += FME7_DoNoise(&FME7sound.Noise,Cycles);
#endif
	return z << 6;
}

int	_MAPINT	FME7sound_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,FME7sound.select)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[0].byte0)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[0].byte1)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[0].byte2)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[0].CurP)
	SAVELOAD_LONG(mode,x,data,FME7sound.Sqr[0].LCtr)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[1].byte0)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[1].byte1)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[1].byte2)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[1].CurP)
	SAVELOAD_LONG(mode,x,data,FME7sound.Sqr[1].LCtr)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[2].byte0)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[2].byte1)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[2].byte2)
	SAVELOAD_BYTE(mode,x,data,FME7sound.Sqr[2].CurP)
	SAVELOAD_LONG(mode,x,data,FME7sound.Sqr[2].LCtr)
#ifdef	FME7_NOISE
	SAVELOAD_BYTE(mode,x,data,FME7sound.Noise.byte0)
	SAVELOAD_WORD(mode,x,data,FME7sound.Noise.ShiftReg.s0)
	SAVELOAD_LONG(mode,x,data,FME7sound.Noise.LCtr)
#endif
	return x;
}
