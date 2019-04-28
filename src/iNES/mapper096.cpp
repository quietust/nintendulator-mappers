/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
uint8_t Pos;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, Latch::Data & 0x3);
	EMU->SetCHR_RAM4(0, (Latch::Data & 0x4) | (Pos & 3));
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
	uint8_t newpos = Addr >> 8;
	if (((Pos & 0x30) != 0x20) && ((newpos & 0x30) == 0x20))
	{
		Pos = newpos;
		Sync();
	}
	else	Pos = (newpos & 0x30) | (Pos & 0x3);
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
	return TRUE;
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

uint16_t MapperNum = 96;
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
