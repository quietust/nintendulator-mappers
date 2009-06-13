/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Latch::Data >> 0) & 0x3);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0, (Latch::Data >> 4) & 0xF);
	else	EMU->SetCHR_RAM8(0, 0);
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

u8 MapperNum = 11;
} // namespace

CTMapperInfo	MapperInfo_011 =
{
	&MapperNum,
	_T("Color Dreams"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
