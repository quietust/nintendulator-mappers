/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
u8 Bank, Mode;

void	Sync (void)
{
	EMU->SetCHR_RAM8(0, 0);
	if (Mode & 1)
		EMU->SetPRG_ROM8(0x6, Bank | 0x23);
	else	EMU->SetPRG_ROM8(0x6, Bank | 0x2F);
	if (Mode == 2)
		EMU->SetPRG_ROM16(0x8, (Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0x8, Bank >> 1);
	if (Mode & 1)
		EMU->SetPRG_ROM16(0xC, (Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0xC, (Bank >> 1) | 7);
	if (Mode == 3)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Bank);
	SAVELOAD_BYTE(mode, x, data, Mode);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write67 (int Bank, int Addr, int Val)
{
	Mode = ((Val & 0x10) >> 3) | ((Val & 0x02) >> 1);
	Sync();
}

void	MAPINT	Write89ABCDEF (int Bank, int Addr, int Val)
{
	Bank = (Val & 0xF) << 2;
	if (Bank & 0x4)
		Mode = (Mode & 0x1) | ((Val & 0x10) >> 3);
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write67);
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x, Write89ABCDEF);

	if (ResetType == RESET_HARD)
	{
		Bank = 0;
		Mode = 1;
	}
	Sync();
}
} // namespace

const MapperInfo MapperInfo_BMC_1992Ballgames11in1 =
{
	"BMC-1992Ballgames11in1",
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
