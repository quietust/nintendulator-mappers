/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, PRG);
	EMU->SetPRG_ROM8(0x8, 0xC);
	EMU->SetPRG_ROM8(0xA, 0xD);
	EMU->SetPRG_RAM8(0xC, 0);
	EMU->SetPRG_ROM8(0xE, 0xF);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}


void	MAPINT	Write (int Bank, int Addr, int Val)
{
	PRG = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		PRG = 0;

	iNES_SetMirroring();
	Sync();
}

uint8 MapperNum = 125;
} // namespace

const MapperInfo MapperInfo_125 =
{
	&MapperNum,
	_T("Monty on the Run (FDS hack)"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
