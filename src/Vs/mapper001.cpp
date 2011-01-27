/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
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

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC1::SaveLoad(mode, offset, data);
	offset = VS::SaveLoad(mode, offset, data);
	return offset;
}

BOOL	MAPINT	Load (void)
{
	VS::Load();
	MMC1::Load(Sync);
	iNES_SetSRAM();
	return TRUE;
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

uint8 MapperNum = 1;
} // namespace

const MapperInfo MapperInfo_001 =
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