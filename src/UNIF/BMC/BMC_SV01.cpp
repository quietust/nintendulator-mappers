/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	EMU->SetCHR_ROM8(0x0, Latch::Addr.b0 & 0x07);
	if (Latch::Addr.b0 & 0x40)
	{
		EMU->SetPRG_ROM16(0x8, (Latch::Addr.b0 & 0x38) >> 3);
		EMU->SetPRG_ROM16(0xC, (Latch::Addr.b0 & 0x38) >> 3);
	}
	else	EMU->SetPRG_ROM32(0x8, (Latch::Addr.b0 & 0x30) >> 4);
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

const MapperInfo MapperInfo_BMC_SV01
(
	"BMC-SV01",
	_T("Pirate multicart mapper"),
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
