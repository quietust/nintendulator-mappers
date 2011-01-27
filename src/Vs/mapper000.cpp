/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
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
	VS::Reset(ResetType);
	iNES_SetMirroring();

	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, 0);
}
void	MAPINT	Unload (void)
{
	VS::Unload();
}

uint8 MapperNum = 0;
} // namespace

const MapperInfo MapperInfo_000 =
{
	&MapperNum,
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
};