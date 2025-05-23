/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, -1);
	EMU->SetPRG_ROM16(0xC, Latch::Data & 0xF);
	EMU->SetCHR_RAM8(0x0, 0);
	switch ((Latch::Data & 0xC0) >> 6)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_A11();	break;
	case 2:	EMU->Mirror_A10();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, TRUE);
	return TRUE;
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

const MapperInfo MapperInfo_097
(
	97,
	_T("Kaiketsu Yanchamaru"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
);
