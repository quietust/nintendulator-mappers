/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_FME7.h"

namespace
{
void	Sync (void)
{
	FME7::SyncPRG(0xFF, 0);
	FME7::SyncCHR(0xFF, 0);
	FME7::SyncMirror();
}

BOOL	MAPINT	Load (void)
{
	FME7::Load(Sync, TRUE);
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	FME7::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	FME7::Unload();
}
} // namespace

const MapperInfo MapperInfo_069
(
	69,
	_T("Sunsoft FME-7"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	FME7::CPUCycle,
	NULL,
	FME7::SaveLoad,
	FME7::GenSound,
	NULL
);
