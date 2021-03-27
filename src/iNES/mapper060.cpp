/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Game;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0x0, Game);
	EMU->SetPRG_ROM16(0x8, Game);
	EMU->SetPRG_ROM16(0xC, Game);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Game);

	if (IsLoad(mode))
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

uint16_t MapperNum = 60;
} // namespace

const MapperInfo MapperInfo_060
(
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
);
