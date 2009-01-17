/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Mapper.Reg1 & 0x7);
	if (Mapper.Reg0 & 0x08)
	{
		EMU->SetPRG_ROM16(0x8,Mapper.Reg0 & 0x7);
		EMU->SetPRG_ROM16(0xC,Mapper.Reg0 & 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8,(Mapper.Reg0 & 0x6) >> 1);
	if (Mapper.Reg0 & 0x10)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Mapper.Reg0 & 0x20)
		return;
	switch (Addr & 1)
	{
	case 0:	Mapper.Reg0 = Val;	break;
	case 1:	Mapper.Reg1 = Val;	break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6,Write);
	Mapper.Reg0 = Mapper.Reg1 = 0;
	Sync();
}

CTMapperInfo	MapperInfo_BMC_WS =
{
	"BMC-WS",
	_T("Pirate multicart mapper"),
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