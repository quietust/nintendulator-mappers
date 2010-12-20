/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_FDS.h"
#include	"..\Hardware\h_FDS.h"

static	void	MAPINT	Load (void)
{
	FDS::Load();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	FDS::Reset(ResetType);
}

static	void	MAPINT	Unload (void)
{
	FDS::Unload();
}

const MapperInfo MapperInfo_FDS =
{
	NULL,
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
};