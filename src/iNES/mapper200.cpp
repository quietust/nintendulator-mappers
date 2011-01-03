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
	EMU->SetPRG_ROM16(0, Latch::Addr.b0 & 0x07);
	EMU->SetPRG_ROM16(0, Latch::Addr.b0 & 0x07);
	EMU->SetCHR_ROM8(0, Latch::Addr.b0 & 0x07);
	if (Latch::Addr.b0 & 0x08)
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

uint8 MapperNum = 200;
} // namespace

const MapperInfo MapperInfo_200 =
{
	&MapperNum,
	_T("36-in-1"),
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