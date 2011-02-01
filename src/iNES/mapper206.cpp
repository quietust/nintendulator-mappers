/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_N118.h"

namespace
{
void	Sync (void)
{
	N118::SyncPRG();
	N118::SyncCHR();
}

BOOL	MAPINT	Load (void)
{
	N118::Load(Sync);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	N118::Reset(ResetType);
	iNES_SetMirroring();
}
void	MAPINT	Unload (void)
{
	N118::Unload();
}

uint8 MapperNum = 206;
} // namespace

const MapperInfo MapperInfo_206 =
{
	&MapperNum,
	_T("DxROM"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad,
	NULL,
	NULL
};