/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	if (Latch::Addr.b0 & 0x40)
	{
		EMU->SetPRG_ROM16(0x8, (Latch::Addr.b0 & 0x7));
		EMU->SetPRG_ROM16(0xC, (Latch::Addr.b0 & 0x7));
	}
	else	EMU->SetPRG_ROM32(0x8, (Latch::Addr.b0 & 0x6) >> 1);
	EMU->SetCHR_ROM8(0x0, (Latch::Addr.b0 & 0x38) >> 3);
	if (Latch::Addr.b0 & 0x80)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
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

const MapperInfo MapperInfo_235
(
	58,
	_T("Mapper 58"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_AL,
	NULL,
	NULL
);
