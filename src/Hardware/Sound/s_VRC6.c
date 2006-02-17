#include	"..\..\interface.h"
#include	"s_VRC6.h"

// Konami VRC6

static	struct	TVRC6sound
{
	struct	VRC6sqr
	{
		union
		{
			struct
			{
				unsigned volume	: 4;
				unsigned duty	: 3;
				unsigned digital: 1;
				unsigned freq	:12;
				unsigned	: 3;
				unsigned enabled: 1;
			};
			struct
			{
				unsigned byte0	: 8;
				unsigned byte1	: 8;
				unsigned byte2	: 8;
			};
		};
		u8 CurP;
		s32 LCtr;
	} Sq0, Sq1;
	struct	VRC6saw
	{
		union
		{
			struct
			{
				unsigned volume	: 6;
				unsigned 	: 2;
				unsigned freq	:12;
				unsigned	: 3;
				unsigned enabled: 1;
			};
			struct
			{
				unsigned byte0	: 8;
				unsigned byte1	: 8;
				unsigned byte2	: 8;
			};
		};
		u8 CurP;
		u8 Acc;
		s32 LCtr;
	} Saw;
}	VRC6sound;

static	int	VRC6_GenerateSaw (struct VRC6saw *ChanData, int Cycles)
{
	ChanData->LCtr -= Cycles;
	while (ChanData->LCtr <= 0)
	{
		ChanData->Acc++;
		ChanData->Acc %= 14;
		if (ChanData->Acc == 0)
			ChanData->CurP = 0;
		else if (!(ChanData->Acc & 1))
			ChanData->CurP += ChanData->volume;
		ChanData->LCtr += ChanData->freq + 1;
	}
	return ((ChanData->CurP >> 3) - 0x10) << 1;
}

static	int	VRC6_GenerateSquare (struct VRC6sqr *ChanData, int Cycles)
{
	ChanData->LCtr -= Cycles;
	while (ChanData->LCtr <= 0)
	{
		ChanData->CurP++;
		ChanData->CurP &= 0xF;
		ChanData->LCtr += ChanData->freq + 1;
	}
	return (((ChanData->digital) || (ChanData->CurP <= ChanData->duty)) ? 1 : -1) * ChanData->volume;
}

void	VRC6sound_Init (void)
{
	memset(&VRC6sound,0,sizeof(VRC6sound));
	VRC6sound.Sq0.LCtr = 1;
	VRC6sound.Sq1.LCtr = 1;
	VRC6sound.Saw.LCtr = 1;
}

void	VRC6sound_Write (int Addr, int Val)
{
	switch (Addr)
	{
	case 0x9000:	VRC6sound.Sq0.byte0 = Val;	break;
	case 0x9001:	VRC6sound.Sq0.byte1 = Val;	break;
	case 0x9002:	VRC6sound.Sq0.byte2 = Val;	break;
	case 0xA000:	VRC6sound.Sq1.byte0 = Val;	break;
	case 0xA001:	VRC6sound.Sq1.byte1 = Val;	break;
	case 0xA002:	VRC6sound.Sq1.byte2 = Val;	break;
	case 0xB000:	VRC6sound.Saw.byte0 = Val;	break;
	case 0xB001:	VRC6sound.Saw.byte1 = Val;	break;
	case 0xB002:	VRC6sound.Saw.byte2 = Val;	break;
	}
}

int	_MAPINT	VRC6sound_Get (int Cycles)
{
	int z = 0;
	if (VRC6sound.Sq0.enabled)	z += VRC6_GenerateSquare(&VRC6sound.Sq0,Cycles);
	if (VRC6sound.Sq1.enabled)	z += VRC6_GenerateSquare(&VRC6sound.Sq1,Cycles);
	if (VRC6sound.Saw.enabled)	z += VRC6_GenerateSaw(&VRC6sound.Saw,Cycles);
	return z << 8;
}

int	_MAPINT	VRC6sound_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq0.byte0)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq0.byte1)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq0.byte2)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq0.CurP)
	SAVELOAD_LONG(mode,x,data,VRC6sound.Sq0.LCtr)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq1.byte0)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq1.byte1)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq1.byte2)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Sq1.CurP)
	SAVELOAD_LONG(mode,x,data,VRC6sound.Sq1.LCtr)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Saw.byte0)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Saw.byte1)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Saw.byte2)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Saw.CurP)
	SAVELOAD_BYTE(mode,x,data,VRC6sound.Saw.Acc)
	SAVELOAD_LONG(mode,x,data,VRC6sound.Saw.LCtr)
	return x;
}

void	VRC6sound_Destroy (void)
{
}