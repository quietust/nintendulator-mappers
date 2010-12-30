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
uint8 Pos;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x3);
	EMU->SetCHR_RAM4(0, (Latch::Data & 0x4) | Pos);
	EMU->SetCHR_RAM4(4, (Latch::Data & 0x4) | 3);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = Latch::SaveLoad_D(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, Pos);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	uint8 newpos;
	if ((Addr & 0x3000) != 0x2000)	return;
	if ((Addr & 0x3FF) >= 0x3C0)	return;
	newpos = (Addr >> 8) & 3;
	if (Pos != newpos)
	{
		Pos = newpos;
		Sync();
	}
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	Latch::Reset(ResetType);
	if (ResetType == RESET_HARD)
	{
		Pos = 0;
		Sync();
	}
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint8 MapperNum = 96;
} // namespace

const MapperInfo MapperInfo_096 =
{
	&MapperNum,
	_T("Oeka Kids"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};