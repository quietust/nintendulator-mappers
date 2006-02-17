#include	"..\..\interface.h"
#include	"s_FME07.h"

// Sunsoft FME-07, based on the AY-8910

#define	NES_INC_SIZE_INT (204800000 / 77)

//#define	FME07_NOISE

typedef	struct	FME07sqr
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
}	TFME07sqr, *PFME07sqr;

#ifdef	FME07_NOISE
typedef	struct	FME07noise
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
}	TFME07noise, *PFME07noise;
#endif

typedef	struct	FME07sound
{
	union
	{
		struct
		{
			unsigned tone      : 3;
#ifdef	FME07_NOISE
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
	TFME07sqr Sqr[3];
#ifdef	FME07_NOISE
	TFME07noise Noise;
#endif
}	TFME07sound, *PFME07sound;

static	TFME07sound	FME07sound;

static	int	FME07_DoSquare (PFME07sqr ChanData, int Cycles)
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
#ifdef	FME07_NOISE
static	int	FME07_DoNoise (PFME07noise ChanData, int Cycles)
{
	int x, volume = 0;
        for (x = 0; x < 3; x++)
	{
		if (!(FME07sound.noise & (1 << x)))
			volume += FME07sound.Sqr[x].volume;	// 1 noise for each channel
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
void	FME07sound_Init (void)
{
	memset(&FME07sound,0,sizeof(FME07sound));
	FME07sound.Sqr[0].LCtr = 1;
	FME07sound.Sqr[1].LCtr = 1;
	FME07sound.Sqr[2].LCtr = 1;
#ifdef	FME07_NOISE
	FME07sound.Noise.ShiftReg.s0 = 0xFFFF;
	FME07sound.Noise.LCtr = 1;
#endif
}

void	FME07sound_Write (int Where, int What)
{
	switch (Where & 0xE000)
	{
	case 0xC000:	FME07sound.select = What & 0xF;	break;	
	case 0xE000:	switch (FME07sound.select)
			{
			case 0x0:	FME07sound.Sqr[0].byte0 = What;	break;
			case 0x1:	FME07sound.Sqr[0].byte1 = What;	break;
			case 0x2:	FME07sound.Sqr[1].byte0 = What;	break;
			case 0x3:	FME07sound.Sqr[1].byte1 = What;	break;
			case 0x4:	FME07sound.Sqr[2].byte0 = What;	break;
			case 0x5:	FME07sound.Sqr[2].byte1 = What;	break;
#ifdef	FME07_NOISE
			case 0x6:	FME07sound.Noise.byte0 = What;	break;
#endif
			case 0x7:	FME07sound.byte7 = What;	break;
			case 0x8:	FME07sound.Sqr[0].byte2 = What;	break;
			case 0x9:	FME07sound.Sqr[1].byte2 = What;	break;
			case 0xA:	FME07sound.Sqr[2].byte2 = What;	break;
			case 0xB:	FME07sound.byteB = What;	break;
			case 0xC:	FME07sound.byteC = What;	break;
			case 0xD:	FME07sound.byteD = What;	break;
			}				break;
	}
}

int	_MAPINT	FME07sound_Get (int Cycles)
{
	int z = 0;
	if (!(FME07sound.tone & 1))	z += FME07_DoSquare(&FME07sound.Sqr[0],Cycles);
	if (!(FME07sound.tone & 2))	z += FME07_DoSquare(&FME07sound.Sqr[1],Cycles);
	if (!(FME07sound.tone & 4))	z += FME07_DoSquare(&FME07sound.Sqr[2],Cycles);
#ifdef	FME07_NOISE
	if (FME07sound.noise ^ 0x7)	z += FME07_DoNoise(&FME07sound.Noise,Cycles);
#endif
	return z << 5;
}

int	_MAPINT	FME07sound_SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,FME07sound.select)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[0].byte0)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[0].byte1)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[0].byte2)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[0].CurP)
	SAVELOAD_LONG(mode,x,data,FME07sound.Sqr[0].LCtr)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[1].byte0)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[1].byte1)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[1].byte2)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[1].CurP)
	SAVELOAD_LONG(mode,x,data,FME07sound.Sqr[1].LCtr)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[2].byte0)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[2].byte1)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[2].byte2)
	SAVELOAD_BYTE(mode,x,data,FME07sound.Sqr[2].CurP)
	SAVELOAD_LONG(mode,x,data,FME07sound.Sqr[2].LCtr)
#ifdef	FME07_NOISE
	SAVELOAD_BYTE(mode,x,data,FME07sound.Noise.byte0)
	SAVELOAD_WORD(mode,x,data,FME07sound.Noise.ShiftReg.s0)
	SAVELOAD_LONG(mode,x,data,FME07sound.Noise.LCtr)
#endif
	return x;
}

void	FME07sound_Destroy (void)
{
}