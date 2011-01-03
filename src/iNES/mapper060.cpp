/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Game;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0, Game);
	EMU->SetPRG_ROM16(0x8, Game);
	EMU->SetPRG_ROM16(0xC, Game);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Game);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	if (ResetType == RESET_HARD)
		Game = 0;
	else	Game++;

	Sync();
}

uint8 MapperNum = 60;
} // namespace

const MapperInfo MapperInfo_060 =
{
	&MapperNum,
	_T("Reset-triggered 4-in-1"),
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