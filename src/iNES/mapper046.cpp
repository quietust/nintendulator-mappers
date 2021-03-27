/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
uint8_t Game;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, ((Game & 0x0F) << 1) | (Latch::Data & 0x01));
	EMU->SetCHR_ROM8(0x0, ((Game & 0xF0) >> 1) | ((Latch::Data & 0x70) >> 4));
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = Latch::SaveLoad_D(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Game);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Game = Val;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);
	if (ResetType == RESET_HARD)
		Game = 0;
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint16_t MapperNum = 46;
} // namespace

const MapperInfo MapperInfo_046
(
	&MapperNum,
	_T("GameStation/RumbleStation"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
