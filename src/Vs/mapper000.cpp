/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
BOOL	MAPINT	Load (void)
{
	VS::Load();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->Mirror_4S();
	VS::Reset(ResetType);

	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0x0, 0);
}
void	MAPINT	Unload (void)
{
	VS::Unload();
}
} // namespace

const MapperInfo MapperInfo_000
(
	(uint16_t)0,
	_T("NROM"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	VS::SaveLoad,
	NULL,
	VS::Config
);
