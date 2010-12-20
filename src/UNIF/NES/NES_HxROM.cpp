/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC6.h"

namespace
{
void	Sync_HKROM (void)
{
	MMC6::SyncPRG(0x3F, 0);
	MMC6::SyncCHR_ROM(0xFF, 0);
	MMC6::SyncMirror();
}

void	MAPINT	Load_HKROM (void)
{
	UNIF_SetSRAM(1024);
	MMC6::Load(Sync_HKROM);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC6::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC6::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_HKROM =
{
	"NES-HKROM",
	_T("MMC6"),
	COMPAT_FULL,
	Load_HKROM,
	Reset,
	Unload,
	NULL,
	MMC6::PPUCycle,
	MMC6::SaveLoad,
	NULL,
	NULL
};