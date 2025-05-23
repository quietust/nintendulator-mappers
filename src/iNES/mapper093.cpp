/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, (Latch::Data & 0xF0) >> 4);
	EMU->SetPRG_ROM16(0xC, -1);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0x0, 0);
	else	EMU->SetCHR_RAM8(0x0, 0);
	if (Latch::Data & 0x01)
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

const MapperInfo MapperInfo_093
(
	93,
	_T("Fantasy Zone"),
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
