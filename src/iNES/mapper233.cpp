/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetCHR_RAM8(0x0, 0);
	if (Latch::Data & 0x20)
	{
		EMU->SetPRG_ROM16(0x8, (Latch::Data & 0x1F));
		EMU->SetPRG_ROM16(0xC, (Latch::Data & 0x1F));
	}
	else	EMU->SetPRG_ROM32(0x8, ((Latch::Data & 0x1E) >> 1));
	switch ((Latch::Data & 0xC0) >> 6)
	{
	case 0:	EMU->Mirror_Custom(0, 0, 0, 1);	break;
	case 1:	EMU->Mirror_A10();		break;
	case 2:	EMU->Mirror_A11();		break;
	case 3:	EMU->Mirror_S1();		break;
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

const MapperInfo MapperInfo_233
(
	233,
	_T("42-in-1"),
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
