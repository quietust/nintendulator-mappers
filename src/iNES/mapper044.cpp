/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t Game;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG((Game == 6) ? 0x1F : 0x0F, Game << 4);
	MMC3::SyncCHR_ROM((Game == 6) ? 0xFF : 0x7F, Game << 7);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Game);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	MMC3::CPUWriteAB(Bank, Addr, Val);	break;
	case 1:	Game = Val & 0x07;
		if (Game == 7)
			Game = 6;
		Sync();				break;
	}
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Game = 0;
	MMC3::Reset(ResetType);
	for (int i = 0xA; i < 0xC; i++)
		EMU->SetCPUWriteHandler(i, Write);	/* need to override writes to $A001 */
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint16_t MapperNum = 44;
} // namespace

const MapperInfo MapperInfo_044
(
	&MapperNum,
	_T("Super HiK 7 in 1 (MMC3)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
