/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Reg[2];

void	Sync (void)
{
	if (Reg[1] & 0x10)
		EMU->SetPRG_ROM32(0x8, (Reg[1] & 0xC0) >> 6);
	else
	{
		EMU->SetPRG_ROM16(0x8, (Reg[1] & 0xE0) >> 5);
		EMU->SetPRG_ROM16(0xC, (Reg[1] & 0xE0) >> 5);
	}
	EMU->SetCHR_ROM8(0, (Reg[0] & 0x7) | (Reg[1] & 0x7) | ((Reg[0] & 0x40) >> 3));
	if (Reg[1] & 0x08)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, Reg[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Reg[1] = Val;
	else	Reg[0] = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(0x8, Write);

	if (ResetType == RESET_HARD)
	{
		Reg[0] = 0;
		Reg[1] = 0;
	}

	Sync();
}

uint8 MapperNum = 57;
} // namespace

const MapperInfo MapperInfo_057 =
{
	&MapperNum,
	_T("GK 6-in-1"),
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