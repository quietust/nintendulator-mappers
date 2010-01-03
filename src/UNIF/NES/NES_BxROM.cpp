/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
void	Sync_BNROM (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x3);
	EMU->SetCHR_RAM8(0, 0);
}

void	MAPINT	Load_BNROM (void)
{
	Latch::Load(Sync_BNROM, TRUE);
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

CTMapperInfo	MapperInfo_NES_BNROM =
{
	"NES-BNROM",
	_T("Standard 32KB PRG switch"),
	COMPAT_FULL,
	Load_BNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};