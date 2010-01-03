/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
int Game;
int Bank;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Game | Bank);
	EMU->SetPRG_ROM16(0xC, Game | 3);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Game);
	SAVELOAD_BYTE(mode, x, data, Bank);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	Game = (Val & 0x18) >> 1;
	Sync();
}

void	MAPINT	WriteABCDEF (int Bank, int Addr, int Val)
{
	Bank = Val & 3;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteABCDEF);
	EMU->SetCPUWriteHandler(0xB, WriteABCDEF);
	EMU->SetCPUWriteHandler(0xC, WriteABCDEF);
	EMU->SetCPUWriteHandler(0xD, WriteABCDEF);
	EMU->SetCPUWriteHandler(0xE, WriteABCDEF);
	EMU->SetCPUWriteHandler(0xF, WriteABCDEF);

	if (ResetType == RESET_HARD)
	{
		Game = 0;
		Bank = 3;
	}

	Sync();
}

u8 MapperNum = 232;
} // namespace

CTMapperInfo	MapperInfo_232 =
{
	&MapperNum,
	_T("Camerica 9096"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};