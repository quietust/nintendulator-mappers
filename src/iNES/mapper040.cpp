/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 IRQenabled;
u16_n IRQcounter;
u8 PRG;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, 6);
	EMU->SetPRG_ROM8(0x8, 4);
	EMU->SetPRG_ROM8(0xA, 5);
	EMU->SetPRG_ROM8(0xC, PRG);
	EMU->SetPRG_ROM8(0xE, 7);
	EMU->SetCHR_ROM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		IRQcounter.s0++;
		if (IRQcounter.s0 & 4096)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	IRQenabled = 0;
	IRQcounter.s0 = 0;
	EMU->SetIRQ(1);
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	IRQenabled = 1;
}

void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	PRG = Val & 7;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);

	if (ResetType == RESET_HARD)
	{
		IRQenabled = 0;
		IRQcounter.s0 = 0;
		PRG = 0;
	}

	Sync();
}

u8 MapperNum = 40;
} // namespace

CTMapperInfo	MapperInfo_040 =
{
	&MapperNum,
	_T("SMB2j Pirate"),
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