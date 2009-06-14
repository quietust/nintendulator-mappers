/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_MMC1.h"
#include	"..\Hardware\h_VS.h"

namespace
{
void	Sync (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	if (ROM->INES_CHRSize)
		MMC1::SyncCHR_ROM(0x1F, 0);
	else	MMC1::SyncCHR_RAM(0x01, 0);
	MMC1::SyncWRAM();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC1::SaveLoad(mode, x, data);
	x = VS::SaveLoad(mode, x, data);
	return x;
}
void	MAPINT	Load (void)
{
	VS::Load();
	MMC1::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	VS::Reset(ResetType);
	MMC1::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC1::Unload();
	VS::Unload();
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
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
};