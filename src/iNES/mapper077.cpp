/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper002.cpp $
 * $Id: mapper002.cpp 1074 2010-12-20 03:30:32Z quietust $
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x0F);
	EMU->SetCHR_ROM2(0, (Latch::Data & 0xF0) >> 4);
	EMU->SetCHR_RAM2(2, 0);
	EMU->SetCHR_RAM2(2, 1);
	EMU->SetCHR_RAM2(2, 2);
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

uint8 MapperNum = 77;
} // namespace

const MapperInfo MapperInfo_077 =
{
	&MapperNum,
	_T("Napoleon Senki"),
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