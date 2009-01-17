/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC4.h"

static	void	Sync (void)
{
	MMC4_SyncPRG(0xF,0);
	MMC4_SyncCHR();
	MMC4_SyncMirror();
	EMU->SetPRG_RAM8(0x6,0);
}

static	void	MAPINT	Load (void)
{
	MMC4_Load(Sync);
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	MMC4_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC4_Unload();
}

static	u8 MapperNum = 10;
CTMapperInfo	MapperInfo_010 =
{
	&MapperNum,
	_T("MMC4"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC4_SaveLoad,
	NULL,
	NULL
};