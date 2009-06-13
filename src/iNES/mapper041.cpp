/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	if (Mapper.Reg0 & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetPRG_ROM32(0x8,Mapper.Reg0 & 0x7);
	EMU->SetCHR_ROM8(0,Mapper.Reg1 | ((Mapper.Reg0 >> 1) & 0xC));
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	WriteLo (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		return;
	Mapper.Reg0 = Addr & 0x3F;
	Sync();
}

static	void	MAPINT	WriteHi (int Bank, int Addr, int Val)
{
	if (Mapper.Reg0 & 0x4)
	{
		Mapper.Reg1 = Val & 0x03;
		Sync();
	}
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x6,WriteLo);
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,WriteHi);

	if (ResetType == RESET_HARD)
		Mapper.Reg0 = Mapper.Reg1 = 0;
	Sync();
}

static	u8 MapperNum = 41;
CTMapperInfo	MapperInfo_041 =
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