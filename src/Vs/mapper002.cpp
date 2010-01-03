/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_Latch.h"
#include	"..\Hardware\h_VS.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch::SaveLoad_D(mode, x, data);
	x = VS::SaveLoad(mode, x, data);
	return x;
}

void	MAPINT	Load (void)
{
	VS::Load();
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	VS::Reset(ResetType);
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
	VS::Unload();
}

u8 MapperNum = 2;
} // namespace

CTMapperInfo	MapperInfo_002 =
{
	&MapperNum,
	_T("UNROM"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
};