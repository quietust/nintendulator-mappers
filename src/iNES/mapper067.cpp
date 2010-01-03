/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRG;
u8 CHR[4];
u8 Mirror;
u16_n IRQcounter;
u8 IRQenabled;
u8 IRQpos;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);

	EMU->SetCHR_ROM2(0, CHR[0]);
	EMU->SetCHR_ROM2(2, CHR[1]);
	EMU->SetCHR_ROM2(4, CHR[2]);
	EMU->SetCHR_ROM2(6, CHR[3]);

	switch (Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_BYTE(mode, x, data, IRQpos);
	SAVELOAD_BYTE(mode, x, data, PRG);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		if (!IRQcounter.s0)
		{
			IRQenabled = 0;
			IRQcounter.s0 = 0xFFFF;
			EMU->SetIRQ(0);
		}
		else	IRQcounter.s0--;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[0] = Val;
		Sync();
	}
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[1] = Val;
		Sync();
	}
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[2] = Val;
		Sync();
	}
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[3] = Val;
		Sync();
	}
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		if (IRQpos)
			IRQcounter.b0 = Val;
		else	IRQcounter.b1 = Val;
		IRQpos ^= 1;
	}
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		IRQpos = 0;
		IRQenabled = (Val & 0x10) >> 4;
		EMU->SetIRQ(1);
	}
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		Mirror = Val;
		Sync();
	}
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		PRG = Val;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		CHR[0] = 0;	CHR[1] = 0;	CHR[2] = 0;	CHR[3] = 0;
		Mirror = 0;
		IRQcounter.s0 = 0;
		IRQenabled = IRQpos = 0;
	}

	Sync();
}

u8 MapperNum = 67;
} // namespace

CTMapperInfo	MapperInfo_067 =
{
	&MapperNum,
	_T("Sunsoft Mapper #3"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
