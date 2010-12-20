/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC4.h"

namespace
{
void	Sync (void)
{
	MMC4::SyncPRG(0xF, 0);
	MMC4::SyncCHR();
	MMC4::SyncMirror();
	EMU->SetPRG_RAM8(0x6, 0);
}

void	MAPINT	Load (void)
{
	MMC4::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	MMC4::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC4::Unload();
}

u8 MapperNum = 10;
} // namespace

const MapperInfo MapperInfo_010 =
{
	&MapperNum,
	_T("MMC4"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC4::SaveLoad,
	NULL,
	NULL
};