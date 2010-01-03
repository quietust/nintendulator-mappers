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
u8 Game;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, ((Game & 0x0F) << 1) | (Latch::Data & 0x01));
	EMU->SetCHR_ROM8(0, ((Game & 0xF0) >> 1) | ((Latch::Data & 0x70) >> 4));
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch::SaveLoad_D(mode, x, data);
	SAVELOAD_BYTE(mode, x, data, Game);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Game = Val;
	Sync();
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();
	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);
	if (ResetType == RESET_HARD)
		Game = 0;
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

u8 MapperNum = 46;
} // namespace

CTMapperInfo	MapperInfo_046 =
{
	&MapperNum,
	_T("GameStation/RumbleStation"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
