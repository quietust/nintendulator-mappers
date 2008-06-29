/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC5.h"

static	void	MAPINT	Load_EKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC5_Load(MMC5WRAM_8KB_0KB);
}

static	void	MAPINT	Load_ELROM (void)
{
	MMC5_Load(MMC5WRAM_0KB_0KB);
}

static	void	MAPINT	Load_ETROM (void)
{
	UNIF_SetSRAM(8192);
	MMC5_Load(MMC5WRAM_8KB_8KB);
}

static	void	MAPINT	Load_EWROM (void)
{
	UNIF_SetSRAM(32768);
	MMC5_Load(MMC5WRAM_32KB_0KB);
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC5_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC5_Unload();
}

CTMapperInfo	MapperInfo_NES_EKROM =
{
	"NES-EKROM",
	_T("MMC5 with 8KB SRAM"),
	COMPAT_FULL,
	Load_EKROM,
	Reset,
	Unload,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	NULL
};
CTMapperInfo	MapperInfo_NES_ELROM =
{
	"NES-ELROM",
	_T("MMC5 with no SRAM"),
	COMPAT_FULL,
	Load_ELROM,
	Reset,
	Unload,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	NULL
};
CTMapperInfo	MapperInfo_NES_ETROM =
{
	"NES-ETROM",
	_T("MMC5 with 16KB SRAM"),
	COMPAT_FULL,
	Load_ETROM,
	Reset,
	Unload,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	NULL
};
CTMapperInfo	MapperInfo_NES_EWROM =
{
	"NES-EWROM",
	_T("MMC5 with 32KB SRAM"),
	COMPAT_FULL,
	Load_EWROM,
	Reset,
	Unload,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	NULL
};