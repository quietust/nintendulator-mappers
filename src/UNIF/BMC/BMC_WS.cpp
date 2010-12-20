/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
u8 Reg0, Reg1;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0, Reg1 & 0x7);
	if (Reg0 & 0x08)
	{
		EMU->SetPRG_ROM16(0x8, Reg0 & 0x7);
		EMU->SetPRG_ROM16(0xC, Reg0 & 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8, (Reg0 & 0x6) >> 1);
	if (Reg0 & 0x10)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Reg0);
	SAVELOAD_BYTE(mode, x, data, Reg1);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Reg0 & 0x20)
		return;
	switch (Addr & 1)
	{
	case 0:	Reg0 = Val;	break;
	case 1:	Reg1 = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6, Write);
	Reg0 = Reg1 = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_BMC_WS =
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