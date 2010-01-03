/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetPRG_ROM32(0x8, 0);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0, 0);
	else	EMU->SetCHR_RAM8(0, 0);
	
	// a few carts, like Family Basic, are effectively NROM with battery-backed RAM
	if (ROM->INES_Flags & 0x02)
		EMU->SetPRG_RAM8(0x6, 0);
}

u8 MapperNum = 0;
} // namespace

CTMapperInfo	MapperInfo_000 =
{
	&MapperNum,
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
};