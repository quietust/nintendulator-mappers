/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
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
	uint8 x, y;
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncWRAM();
	for (x = 0; x < 8; x++)
	{
		y = MMC3::GetCHRBank(x);
		if ((y & 0xFE) == ((ROM->INES_PRGSize >> 2) & 0x08))
			EMU->SetCHR_RAM1(x, y & 0x01);
		else	EMU->SetCHR_ROM1(x, y & 0xFF);
	}
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 74;
} // namespace

const MapperInfo MapperInfo_074 =
{
	&MapperNum,
	_T("Mapper 74 (Taiwan MMC3)"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
};