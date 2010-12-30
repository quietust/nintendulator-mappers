/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper225.cpp $
 * $Id: mapper225.cpp 1074 2010-12-20 03:30:32Z quietust $
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM32(0, Latch::Addr.b0);
	EMU->SetCHR_ROM8(0, Latch::Addr.b0);
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

uint8 MapperNum = 201;
} // namespace

const MapperInfo MapperInfo_201 =
{
	&MapperNum,
	_T("21-in-1"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_AL,
	NULL,
	NULL
};