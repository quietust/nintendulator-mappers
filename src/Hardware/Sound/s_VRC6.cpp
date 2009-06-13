/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\interface.h"
#include	"s_VRC6.h"

// Konami VRC6
namespace VRC6sound
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

	int	Generate (int Cycles)
	{
		LCtr -= Cycles;
		while (LCtr <= 0)
		{
			CurP++;
			CurP &= 0xF;
			LCtr += freq + 1;
		}
		return (((digital) || (CurP <= duty)) ? 1 : -1) * volume;
	}

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

	int	Generate (int Cycles)
	{
		LCtr -= Cycles;
		while (LCtr <= 0)
		{
			Acc++;
			Acc %= 14;
			if (Acc == 0)
				CurP = 0;
			else if (!(Acc & 1))
				CurP += volume;
			LCtr += freq + 1;
		}
		return ((CurP >> 3) - 0x10) << 1;
	}
} Saw;

void	Load (void)
{
	ZeroMemory(&Sq0, sizeof(Sq0));
	ZeroMemory(&Sq1, sizeof(Sq1));
	ZeroMemory(&Saw, sizeof(Saw));
}

void	Reset (RESET_TYPE ResetType)
{
	Sq0.LCtr = 1;
	Sq1.LCtr = 1;
	Saw.LCtr = 1;
}

void	Unload (void)
{
}

void	Write (int Addr, int Val)
{
	switch (Addr)
	{
	case 0x9000:	Sq0.byte0 = Val;	break;
	case 0x9001:	Sq0.byte1 = Val;	break;
	case 0x9002:	Sq0.byte2 = Val;	break;
	case 0xA000:	Sq1.byte0 = Val;	break;
	case 0xA001:	Sq1.byte1 = Val;	break;
	case 0xA002:	Sq1.byte2 = Val;	break;
	case 0xB000:	Saw.byte0 = Val;	break;
	case 0xB001:	Saw.byte1 = Val;	break;
	case 0xB002:	Saw.byte2 = Val;	break;
	}
}

int	MAPINT	Get (int Cycles)
{
	int z = 0;
	if (Sq0.enabled)	z += Sq0.Generate(Cycles);
	if (Sq1.enabled)	z += Sq1.Generate(Cycles);
	if (Saw.enabled)	z += Saw.Generate(Cycles);
	return z << 8;
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Sq0.byte0);
	SAVELOAD_BYTE(mode, x, data, Sq0.byte1);
	SAVELOAD_BYTE(mode, x, data, Sq0.byte2);
	SAVELOAD_BYTE(mode, x, data, Sq0.CurP);
	SAVELOAD_LONG(mode, x, data, Sq0.LCtr);

	SAVELOAD_BYTE(mode, x, data, Sq1.byte0);
	SAVELOAD_BYTE(mode, x, data, Sq1.byte1);
	SAVELOAD_BYTE(mode, x, data, Sq1.byte2);
	SAVELOAD_BYTE(mode, x, data, Sq1.CurP);
	SAVELOAD_LONG(mode, x, data, Sq1.LCtr);

	SAVELOAD_BYTE(mode, x, data, Saw.byte0);
	SAVELOAD_BYTE(mode, x, data, Saw.byte1);
	SAVELOAD_BYTE(mode, x, data, Saw.byte2);
	SAVELOAD_BYTE(mode, x, data, Saw.CurP);
	SAVELOAD_BYTE(mode, x, data, Saw.Acc);
	SAVELOAD_LONG(mode, x, data, Saw.LCtr);
	return x;
}
} // namespace VRC6sound