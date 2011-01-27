/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
void	Sync (void)
{
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
	else	MMC3::SyncMirror();
	MMC3::SyncWRAM();	// assume WRAM is here
	MMC3::SyncPRG(0x3F, 0);
	if (ROM->INES_CHRSize)
		MMC3::SyncCHR_ROM(0xFF, 0);
	else	MMC3::SyncCHR_RAM(0x07, 0);
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 4;
} // namespace

const MapperInfo MapperInfo_004 =
{
	&MapperNum,
	_T("MMC3/MMC6"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
};
