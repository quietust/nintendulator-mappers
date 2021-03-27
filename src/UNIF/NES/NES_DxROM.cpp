/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_N118.h"

namespace
{
void	Sync_DEROM (void)
{
	N118::SyncPRG();
	N118::SyncCHR();
	UNIF_SetMirroring(NULL);
}
void	Sync_DEIROM (void)
{
	N118::SyncPRG();
	N118::SyncCHR();
	UNIF_SetMirroring(NULL);
}
void	Sync_DRROM (void)
{
	N118::SyncPRG();
	N118::SyncCHR();
	EMU->Mirror_4();
}

BOOL	MAPINT	Load_DEROM (void)
{
	N118::Load(Sync_DEROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_DEIROM (void)
{
	N118::Load(Sync_DEIROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_DRROM (void)
{
	N118::Load(Sync_DRROM, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	N118::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N118::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_DEROM
(
	"NES-DEROM",
	_T("Namcot 109"),
	COMPAT_NEARLY,
	Load_DEROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad_Alt,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_DEIROM
(
	"NES-DEIROM",
	_T("Namcot 109"),
	COMPAT_NEARLY,
	Load_DEIROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad_Alt,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_DRROM
(
	"NES-DRROM",
	_T("Namcot 109 with 4-screen VRAM"),
	COMPAT_NEARLY,
	Load_DRROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad_Alt,
	NULL,
	NULL
);
