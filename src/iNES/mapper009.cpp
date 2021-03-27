/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC2.h"

namespace
{
void	Sync (void)
{
	MMC2::SyncPRG(0xF, 0);
	MMC2::SyncCHR();
	MMC2::SyncMirror();
}

BOOL	MAPINT	Load (void)
{
	MMC2::Load(Sync, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	MMC2::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC2::Unload();
}

uint16_t MapperNum = 9;
} // namespace

const MapperInfo MapperInfo_009
(
	&MapperNum,
	_T("MMC2"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2::SaveLoad,
	NULL,
	NULL
);
