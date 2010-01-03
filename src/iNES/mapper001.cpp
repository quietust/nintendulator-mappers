/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
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
	if (ROM->INES_PRGSize == 32)
		MMC1::SyncPRG(0xF, MMC1::GetCHRBankLo() & 0x10);
	else	MMC1::SyncPRG(0xF, 0);
	if (ROM->INES_CHRSize)
		MMC1::SyncCHR_ROM(0x1F, 0);
	else	MMC1::SyncCHR_RAM(0x01, 0);
	MMC1::SyncWRAM();
}

void	MAPINT	Load (void)
{
	MMC1::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC1::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC1::Unload();
}

u8 MapperNum = 1;
} // namespace

CTMapperInfo	MapperInfo_001 =
{
	&MapperNum,
	_T("MMC1"),
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