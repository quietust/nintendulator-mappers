/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC2.h"

namespace
{
void	Sync_PEEOROM (void)
{
	MMC2::SyncPRG(0xF, 0);
	MMC2::SyncCHR();
	MMC2::SyncMirror();
}
void	Sync_PNROM (void)
{
	MMC2::SyncPRG(0xF, 0);
	MMC2::SyncCHR();
	MMC2::SyncMirror();
}

void	MAPINT	Load_PEEOROM (void)
{
	MMC2::Load(Sync_PEEOROM);
}
void	MAPINT	Load_PNROM (void)
{
	MMC2::Load(Sync_PNROM);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC2::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC2::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_PEEOROM =
{
	"NES-PEEOROM",
	_T("MMC2 with EPROM support"),
	COMPAT_FULL,
	Load_PNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2::SaveLoad,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_PNROM =
{
	"NES-PNROM",
	_T("MMC2"),
	COMPAT_FULL,
	Load_PNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2::SaveLoad,
	NULL,
	NULL
};
