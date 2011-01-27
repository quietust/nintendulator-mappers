/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
void	Sync_UNROM (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data & 0x7);
	EMU->SetPRG_ROM16(0xC, 0x7);
	EMU->SetCHR_RAM8(0, 0);
}
void	Sync_UOROM (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data & 0xF);
	EMU->SetPRG_ROM16(0xC, 0xF);
	EMU->SetCHR_RAM8(0, 0);
}

BOOL	MAPINT	Load_UNROM (void)
{
	Latch::Load(Sync_UNROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_UOROM (void)
{
	Latch::Load(Sync_UOROM, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
	UNIF_SetMirroring(NULL);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_UNROM =
{
	"NES-UNROM",
	_T("Standard 16KB PRG switch"),
	COMPAT_FULL,
	Load_UNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_UOROM =
{
	"NES-UOROM",
	_T("Oversize 16KB PRG switch"),
	COMPAT_FULL,
	Load_UOROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};