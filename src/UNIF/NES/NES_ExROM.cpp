/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC5.h"

namespace
{
void	MAPINT	Load_EKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC5::Load(MMC5WRAM_8KB_0KB);
}

void	MAPINT	Load_ELROM (void)
{
	MMC5::Load(MMC5WRAM_0KB_0KB);
}

void	MAPINT	Load_ETROM (void)
{
	UNIF_SetSRAM(8192);
	MMC5::Load(MMC5WRAM_8KB_8KB);
}

void	MAPINT	Load_EWROM (void)
{
	UNIF_SetSRAM(32768);
	MMC5::Load(MMC5WRAM_32KB_0KB);
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC5::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC5::Unload();
}
} // namespace

CTMapperInfo	MapperInfo_NES_EKROM =
{
	"NES-EKROM",
	_T("MMC5 with 8KB SRAM"),
	COMPAT_FULL,
	Load_EKROM,
	Reset,
	Unload,
	NULL,
	MMC5::PPUCycle,
	MMC5::SaveLoad,
	MMC5::MapperSnd,
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
	MMC5::PPUCycle,
	MMC5::SaveLoad,
	MMC5::MapperSnd,
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
	MMC5::PPUCycle,
	MMC5::SaveLoad,
	MMC5::MapperSnd,
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
	MMC5::PPUCycle,
	MMC5::SaveLoad,
	MMC5::MapperSnd,
	NULL
};