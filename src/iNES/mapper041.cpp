/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 Reg0, Reg1;

void	Sync (void)
{
	if (Reg0 & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetPRG_ROM32(0x8, Reg0 & 0x7);
	EMU->SetCHR_ROM8(0, Reg1 | ((Reg0 >> 1) & 0xC));
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Reg0);
	SAVELOAD_BYTE(mode, x, data, Reg1);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	WriteLo (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		return;
	Reg0 = Addr & 0x3F;
	Sync();
}

void	MAPINT	WriteHi (int Bank, int Addr, int Val)
{
	if (Reg0 & 0x4)
	{
		Reg1 = Val & 0x03;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x6, WriteLo);
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x, WriteHi);

	if (ResetType == RESET_HARD)
		Reg0 = Reg1 = 0;
	Sync();
}

u8 MapperNum = 41;
} // namespace

const MapperInfo MapperInfo_041 =
{
	&MapperNum,
	_T("Caltron 6-in-1"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};