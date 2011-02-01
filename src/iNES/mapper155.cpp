/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"

namespace
{
void	Sync (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
	// no PRG RAM disable in this version
	EMU->SetPRG_RAM8(0x6, 0);
}

BOOL	MAPINT	Load (void)
{
	MMC1::Load(Sync);
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC1::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC1::Unload();
}

uint8 MapperNum = 155;
} // namespace

const MapperInfo MapperInfo_155 =
{
	&MapperNum,
	_T("MMC1A"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
};