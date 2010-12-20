/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
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

void	MAPINT	Load_FJROM (void)
{
	UNIF_SetSRAM(8192);
	MMC4::Load(Sync_FJROM);
}
void	MAPINT	Load_FKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC4::Load(Sync_FKROM);
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

const MapperInfo MapperInfo_NES_FJROM =
{
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
};
const MapperInfo MapperInfo_NES_FKROM =
{
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
};