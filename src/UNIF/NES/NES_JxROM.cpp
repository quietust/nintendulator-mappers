/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_FME7.h"

namespace
{
void	Sync_JLROM (void)
{
	FME7::SyncPRG(0x3F, 0);
	FME7::SyncCHR(0xFF, 0);
	FME7::SyncMirror();
}

void	Sync_JSROM (void)
{
	FME7::SyncPRG(0x3F, 0);
	FME7::SyncCHR(0xFF, 0);
	FME7::SyncMirror();
}

void	MAPINT	Load_JLROM (void)
{
	FME7::Load(Sync_JLROM);
}
void	MAPINT	Load_JSROM (void)
{
	FME7::Load(Sync_JSROM);
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

CTMapperInfo	MapperInfo_NES_JLROM =
{
	"NES-JLROM",
	_T("Sunsoft FME-7"),
	COMPAT_FULL,
	Load_JLROM,
	Reset,
	Unload,
	FME7::CPUCycle,
	NULL,
	FME7::SaveLoad,
	FME7::GenSound,
	NULL
};
CTMapperInfo	MapperInfo_NES_JSROM =
{
	"NES-JSROM",
	_T("Sunsoft FME-7 with WRAM"),
	COMPAT_FULL,
	Load_JSROM,
	Reset,
	Unload,
	FME7::CPUCycle,
	NULL,
	FME7::SaveLoad,
	FME7::GenSound,
	NULL
};CTMapperInfo	MapperInfo_NES_BTR =
{
	"NES-BTR",
	_T("Sunsoft FME-7 with WRAM (NES-JSROM)"),
	COMPAT_FULL,
	Load_JSROM,
	Reset,
	Unload,
	FME7::CPUCycle,
	NULL,
	FME7::SaveLoad,
	FME7::GenSound,
	NULL
};