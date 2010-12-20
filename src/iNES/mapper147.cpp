/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Reg;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0, (Reg & 0x78) >> 3);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Reg);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if ((Addr & 0x103) == 0x102)
		Reg = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);
	for (int x = 0xC; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}

uint8 MapperNum = 147;
} // namespace

const MapperInfo MapperInfo_147 =
{
	&MapperNum,
	_T("Sachen (TC-U01-1.5M)"),
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