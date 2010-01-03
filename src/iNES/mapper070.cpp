/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
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
	EMU->SetCHR_ROM8(0, Latch::Data & 0xF);
	if (Latch::Data & 0x80)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

u8 MapperNum = 70;
} // namespace

CTMapperInfo	MapperInfo_070 =
{
	&MapperNum,
	_T("Bandai 74161/7432"),
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