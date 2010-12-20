/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Reg;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg >> 4) & 0x3);
	EMU->SetCHR_ROM8(0, Reg & 0xF);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Reg);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Reg = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;

	Sync();
}

uint8 MapperNum = 140;
} // namespace

const MapperInfo MapperInfo_140 =
{
	&MapperNum,
	_T("Mapper 140 (Bio Senshi Dan)"),
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