/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
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
	EMU->SetCHR_RAM8(0, 0);
	if (Latch::Addr.b0 & 0x20)
		EMU->SetPRG_ROM32(0x8, (Latch::Addr.b0 & 0x1E) >> 1);
	else
	{
		EMU->SetPRG_ROM16(0x8, Latch::Addr.b0 & 0x1E);
		EMU->SetPRG_ROM16(0xC, Latch::Addr.b0 & 0x1E);
	}
	switch ((Latch::Addr.b0 & 0xC0) >> 6)
	{
	case 0:	EMU->Mirror_S0();		break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_H();		break;
	case 3:	EMU->Mirror_Custom(0, 1, 1, 1);	break;
	}
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

u8 MapperNum = 231;
} // namespace

CTMapperInfo	MapperInfo_231 =
{
	&MapperNum,
	_T("20-in-1"),
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