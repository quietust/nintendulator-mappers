/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Byte8000, Byte8001;
}	Mapper;

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_VH  : 1;
			unsigned PRGhi   : 2;
			unsigned         : 7;
		};
		struct
		{
			u8 b[2];
		};
	}	Val;

	Val.b0 = Mapper.Byte8000;
	Val.b1 = Mapper.Byte8001;

	if (Val.Mir_VH)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (Val.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,((Val.PRGhi) << 5) | (Val.PRGbank));
		EMU->SetPRG_ROM16(0xC,((Val.PRGhi) << 5) | (Val.PRGbank));
	}
	else	EMU->SetPRG_ROM32(0x8,((Val.PRGhi) << 4) | (Val.PRGbank >> 1));
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte8000);
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte8001);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Mapper.Byte8000 = Val;		break;
	case 1:	Mapper.Byte8001 = Val;		break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
		Mapper.Byte8000 = Mapper.Byte8001 = 0;

	Sync();
}

static	u8 MapperNum = 226;
CTMapperInfo	MapperInfo_226 =
{
	&MapperNum,
	_T("76-in-1"),
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