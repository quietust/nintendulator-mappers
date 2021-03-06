/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC4.h"

namespace
{
void	Sync_FJROM (void)
{
	MMC4::SyncPRG(0xF, 0);
	MMC4::SyncCHR();
	MMC4::SyncMirror();
	EMU->SetPRG_RAM8(0x6, 0);
}
void	Sync_FKROM (void)
{
	MMC4::SyncPRG(0xF, 0);
	MMC4::SyncCHR();
	MMC4::SyncMirror();
	EMU->SetPRG_RAM8(0x6, 0);
}

BOOL	MAPINT	Load_FJROM (void)
{
	UNIF_SetSRAM(8192);
	MMC4::Load(Sync_FJROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_FKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC4::Load(Sync_FKROM, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC4::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC4::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_FJROM
(
	"NES-FJROM",
	_T("MMC4"),
	COMPAT_FULL,
	Load_FJROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC4::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_FKROM
(
	"NES-FKROM",
	_T("MMC4"),
	COMPAT_FULL,
	Load_FKROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC4::SaveLoad,
	NULL,
	NULL
);
