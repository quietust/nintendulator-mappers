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
void	Sync_AMROM (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x3);
	EMU->SetCHR_RAM8(0, 0);
	if (Latch::Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}
void	Sync_ANROM (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x3);
	EMU->SetCHR_RAM8(0, 0);
	if (Latch::Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}
void	Sync_AOROM (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x7);
	EMU->SetCHR_RAM8(0, 0);
	if (Latch::Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

void	MAPINT	Load_AMROM (void)
{
	Latch::Load(Sync_AMROM, TRUE);
}
void	MAPINT	Load_ANROM (void)
{
	Latch::Load(Sync_ANROM, FALSE);
}
void	MAPINT	Load_AOROM (void)
{
	Latch::Load(Sync_AOROM, TRUE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}
} // namespace

CTMapperInfo	MapperInfo_NES_AMROM =
{
	"NES-AMROM",
	_T("32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_AMROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_ANROM =
{
	"NES-ANROM",
	_T("Standard 32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_ANROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_AOROM =
{
	"NES-AOROM",
	_T("Oversize 32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_AOROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};