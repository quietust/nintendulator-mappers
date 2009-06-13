/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC6.h"

static	void	Sync_HKROM (void)
{
	MMC6_SyncPRG(0x3F,0);
	MMC6_SyncCHR_ROM(0xFF,0);
	MMC6_SyncMirror();
}

static	void	MAPINT	Load_HKROM (void)
{
	UNIF_SetSRAM(1024);
	MMC6_Load(Sync_HKROM);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC6_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC6_Unload();
}

CTMapperInfo	MapperInfo_NES_HKROM =
{
	"NES-HKROM",
	_T("MMC6"),
	COMPAT_FULL,
	Load_HKROM,
	Reset,
	Unload,
	NULL,
	MMC6_PPUCycle,
	MMC6_SaveLoad,
	NULL,
	NULL
};