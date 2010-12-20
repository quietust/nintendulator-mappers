/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
uint8 Game;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, ((Game & 0xF) << 4) | 0xF);
	if (Game & 0x10)
	{
		EMU->SetPRG_ROM16(0x8, ((Game & 0xF) << 3) | (Latch::Data & 0x7));
		EMU->SetPRG_ROM16(0xC, ((Game & 0xF) << 3) | 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8, 0x40); /* the EPROM at the end */
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
} // namespace

const MapperInfo MapperInfo_BMC_Supervision16in1 =
{
	"BMC-Supervision16in1",
	_T("Pirate multicart mapper"),
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