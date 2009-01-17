/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	int WhichGame;
	int WhichBank;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.WhichGame | Mapper.WhichBank);
	EMU->SetPRG_ROM16(0xC,Mapper.WhichGame | 3);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichGame);
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichBank);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	Mapper.WhichGame = (Val & 0x18) >> 1;
	Sync();
}

static	void	MAPINT	WriteABCDEF (int Bank, int Addr, int Val)
{
	Mapper.WhichBank = Val & 3;
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xB,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xC,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xD,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xE,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xF,WriteABCDEF);

	if (ResetType == RESET_HARD)
	{
		Mapper.WhichGame = 0;
		Mapper.WhichBank = 3;
	}

	Sync();
}

static	u8 MapperNum = 232;
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