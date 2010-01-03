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

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, PRG);
	EMU->SetPRG_ROM8(0x8, 0xC);
	EMU->SetPRG_ROM8(0xA, 0xD);
	EMU->SetPRG_RAM8(0xC, 0);
	EMU->SetPRG_ROM8(0xE, 0xF);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}


void	MAPINT	Write (int Bank, int Addr, int Val)
{
	PRG = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
		PRG = 0;

	iNES_SetMirroring();
	Sync();
}

u8 MapperNum = 125;
} // namespace

CTMapperInfo	MapperInfo_125 =
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
