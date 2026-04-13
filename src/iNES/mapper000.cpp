/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetPRG_ROM32(0x8, 0);
	// If no CHR ROM is present, use RAM
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0x0, 0);
	else	EMU->SetCHR_RAM8(0x0, 0);
	
	// a few carts, like Family Basic, are effectively NROM with battery-backed RAM
	if (ROM->INES_Flags & 0x02)
		EMU->SetPRG_RAM8(0x6, 0);
	// also add RAM if the NES 2.0 header says we have any
	// (it probably won't handle <4KB, but that's fine for now)
	else if ((ROM->INES_Version == 2) && ((ROM->INES2_PRGRAM & 0x0F) != 0))
		EMU->SetPRG_RAM8(0x6, 0);
}
} // namespace

const MapperInfo MapperInfo_000
(
	(uint16_t)0,
	_T("NROM"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
);
