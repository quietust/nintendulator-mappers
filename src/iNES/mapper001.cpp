/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
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
	else	MMC1::SyncCHR_RAM(0x1F, 0);
	if (ROM->INES_Version == 2)
	{
		if (MMC1::GetWRAMEnabled())
		{
			int bank = 0;
			if (ROM->INES2_PRGRAM == 0x77)	// SOROM
			{
				// bank 1 is batteried while bank 0 is not, so we need to switch it around
				bank = !(MMC1::GetCHRBankLo() & 0x08);
			}
			else if ((ROM->INES2_PRGRAM == 0x90) || (ROM->INES2_PRGRAM == 0x09))	// SXROM, with or without battery
				bank = (MMC1::GetCHRBankLo() & 0x0C) >> 2;
			else //if (ROM->INES2_PRGRAM == 0x70)	// everything else
				bank = 0;
			EMU->SetPRG_RAM8(0x6, bank);
		}
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
	}
	else	MMC1::SyncWRAM();
}

BOOL	MAPINT	Load (void)
{
	MMC1::Load(Sync, TRUE, FALSE);
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
} // namespace

const MapperInfo MapperInfo_001
(
	1,
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
);
