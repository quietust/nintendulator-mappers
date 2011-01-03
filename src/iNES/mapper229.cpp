/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	if (Latch::Addr.b0 & 0x1E)
	{
		EMU->SetPRG_ROM16(0x8, Latch::Addr.b0 & 0x1F);
		EMU->SetPRG_ROM16(0xC, Latch::Addr.b0 & 0x1F);
	}
	else	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, Latch::Addr.b0 & 0x1F);
	if (Latch::Addr.b0 & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint8 MapperNum = 229;
} // namespace

const MapperInfo MapperInfo_229 =
{
	&MapperNum,
	_T("31-in-1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_AL,
	NULL,
	NULL
};