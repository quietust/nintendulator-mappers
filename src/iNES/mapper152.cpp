/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper011.cpp $
 * $Id: mapper011.cpp 1074 2010-12-20 03:30:32Z quietust $
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, (Latch::Data & 0x70) >> 4);
	EMU->SetPRG_ROM16(0xC, -1);
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

uint8 MapperNum = 152;
} // namespace

const MapperInfo MapperInfo_152 =
{
	&MapperNum,
	_T("Arkanoid 2"),
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