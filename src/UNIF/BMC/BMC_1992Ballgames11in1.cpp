/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
uint8 Bank, Mode;

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

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Bank);
	SAVELOAD_BYTE(mode, offset, data, Mode);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
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
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write67);
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write89ABCDEF);

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
