/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_FDS.h"
#include	"..\Hardware\h_FDS.h"

namespace
{
BOOL	MAPINT	Load (void)
{
	return FDS::Load();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	FDS::Reset(ResetType);
}

void	MAPINT	Unload (void)
{
	FDS::Unload();
}
} // namespace

const MapperInfo MapperInfo_FDS
(
	FDS_MAPPERNAME,
	_T("Famicom Disk System"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	FDS::CPUCycle,
	NULL,
	FDS::SaveLoad,
	FDS::MapperSnd,
	FDS::Config
);
