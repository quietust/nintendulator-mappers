/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 Reg;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, Reg >> 7);
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
	if (Addr & 0x100)
		Reg = Val & 0x80;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}

u8 MapperNum = 145;
} // namespace

CTMapperInfo	MapperInfo_145 =
{
	&MapperNum,
	_T("Sachen (SA-72007)"),
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