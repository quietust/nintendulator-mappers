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
uint8 Game;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, (((Game & 0xF) << 4) | 0xF) + 4);
	if (Game & 0x10)
	{
		EMU->SetPRG_ROM16(0x8, (((Game & 0xF) << 3) | (Latch::Data & 0x7)) + 2);
		EMU->SetPRG_ROM16(0xC, (((Game & 0xF) << 3) | 0x7) + 2);
	}
	else	EMU->SetPRG_ROM32(0x8, 0); // the EPROM at the end, now inconveniently at the BEGINNING
	EMU->SetCHR_RAM8(0, 0);
	if (Game & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = Latch::SaveLoad_D(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, Game);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Game & 0x10)
		return;
	Game = Val & 0x3F;
	Sync();
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Game = 0;
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint8 MapperNum = 53;
} // namespace

const MapperInfo MapperInfo_053 =
{
	&MapperNum,
	_T("Supervision 16-in-1"),
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