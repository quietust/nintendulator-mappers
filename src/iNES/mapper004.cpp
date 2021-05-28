/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"
#include	"..\Hardware\h_MMC6.h"

namespace
{
bool IsMMC6 = false;
void	Sync3 (void)
{
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
	else	MMC3::SyncMirror();
	if (ROM->INES_Version == 2)
	{
		if ((ROM->INES2_PRGRAM == 0x70) || (ROM->INES2_PRGRAM == 0x07))	// 8KB, with or without battery
			MMC3::SyncWRAM();
		// otherwise, leave $6000-$7FFF unmapped - some games require this
	}
	else	MMC3::SyncWRAM();	// assume WRAM is here
	MMC3::SyncPRG(0x3F, 0);
	if (ROM->INES_CHRSize)
		MMC3::SyncCHR_ROM(0xFF, 0);
	else	MMC3::SyncCHR_RAM(0xFF, 0);
}

void	Sync6 (void)
{
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
	else	MMC6::SyncMirror();
	// MMC6 always has builtin RAM
	MMC6::SyncPRG(0x3F, 0);
	if (ROM->INES_CHRSize)
		MMC6::SyncCHR_ROM(0xFF, 0);
	else	MMC6::SyncCHR_RAM(0xFF, 0);
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (IsMMC6)
		return MMC6::PPUCycle(Addr, Scanline, Cycle, IsRendering);
	else	return MMC3::PPUCycle(Addr, Scanline, Cycle, IsRendering);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	// MMC3 and MMC6 ver=0 data is largely identical
	if (IsMMC6)
		return MMC6::SaveLoad(mode, offset, data);
	else	return MMC3::SaveLoad(mode, offset, data);
}

BOOL	MAPINT	Load (void)
{
	IsMMC6 = (ROM->INES2_SubMapper == 1);
	if (IsMMC6)
		MMC6::Load(Sync6, TRUE);
	else	MMC3::Load(Sync3, TRUE);
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (IsMMC6)
		MMC6::Reset(ResetType);
	else	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	if (IsMMC6)
		MMC6::Unload();
	else	MMC3::Unload();
}

uint16_t MapperNum = 4;
} // namespace

const MapperInfo MapperInfo_004
(
	&MapperNum,
	_T("MMC3/MMC6"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
