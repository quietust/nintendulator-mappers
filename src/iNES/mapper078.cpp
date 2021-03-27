/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data & 0x7);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM8(0x0, (Latch::Data & 0xF0) >> 4);
	if (Latch::Data & 0x8)
		((ROM->INES_Flags & 0x08) ? EMU->Mirror_V : EMU->Mirror_S1)();
	else	((ROM->INES_Flags & 0x08) ? EMU->Mirror_H : EMU->Mirror_S0)();
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

uint16_t MapperNum = 78;
} // namespace

const MapperInfo MapperInfo_078
(
	&MapperNum,
	_T("Irem 74161/32"),
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
