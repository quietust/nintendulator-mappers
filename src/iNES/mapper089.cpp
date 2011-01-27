/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
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
	EMU->SetPRG_ROM16(0x8, (Latch::Data & 0x70) >> 4);
	EMU->SetPRG_ROM16(0xC, 0x7);
	EMU->SetCHR_ROM8(0, (Latch::Data & 0x7) | ((Latch::Data & 0x80) >> 4));
	if (Latch::Data & 0x08)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint8 MapperNum = 89;
} // namespace

const MapperInfo MapperInfo_089 =
{
	&MapperNum,
	_T("Sunsoft 74161/7432"),
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