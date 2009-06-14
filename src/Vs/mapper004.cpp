/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_MMC3.h"
#include	"..\Hardware\h_VS.h"

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

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3::SaveLoad(mode, x, data);
	x = VS::SaveLoad(mode, x, data);
	return x;
}

void	MAPINT	Load (void)
{
	VS::Load();
	MMC3::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	VS::Reset(ResetType);
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
	VS::Unload();
}

u8 MapperNum = 4;
} // namespace

CTMapperInfo	MapperInfo_004 =
{
	&MapperNum,
	_T("MMC3/MMC6"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	VS::Config
};
