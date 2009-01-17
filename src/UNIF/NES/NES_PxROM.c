/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC2.h"

static	void	Sync_PNROM (void)
{
	MMC2_SyncPRG(0xF,0);
	MMC2_SyncCHR();
	MMC2_SyncMirror();
}

static	void	MAPINT	Load_PNROM (void)
{
	MMC2_Load(Sync_PNROM);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC2_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC2_Unload();
}

CTMapperInfo	MapperInfo_NES_PNROM =
{
	"NES-PNROM",
	_T("MMC2"),
	COMPAT_FULL,
	Load_PNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2_SaveLoad,
	NULL,
	NULL
};
