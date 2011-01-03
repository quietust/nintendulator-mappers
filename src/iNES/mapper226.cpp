/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Reg0, Reg1;

void	Sync (void)
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
			unsigned byte0   : 8;
			unsigned byte1   : 8;
		};
	};

	byte0 = Reg0;
	byte1 = Reg1;

	if (Mir_VH)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRGhi << 5) | PRGbank);
		EMU->SetPRG_ROM16(0xC, (PRGhi << 5) | PRGbank);
	}
	else	EMU->SetPRG_ROM32(0x8, (PRGhi << 4) | (PRGbank >> 1));
	EMU->SetCHR_RAM8(0x0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Reg0);
	SAVELOAD_BYTE(mode, offset, data, Reg1);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Reg0 = Val;		break;
	case 1:	Reg1 = Val;		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Reg0 = Reg1 = 0;

	Sync();
}

uint8 MapperNum = 226;
} // namespace

const MapperInfo MapperInfo_226 =
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