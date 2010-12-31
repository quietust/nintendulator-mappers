/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Mode;
uint8 PRG[2];
uint8 CHR[8];

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM8(0x8, (Mode & 2) ? -2 : PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, (Mode & 2) ? PRG[0] : -2);
	EMU->SetPRG_ROM8(0xE, -1);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i]);
	if (Mode & 0x1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Mode);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mode = Val;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	CHR[Addr & 7] = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;
		PRG[1] = 1;
		for (int i = 0; i < 8; i++)
			CHR[i] = 0;
		Mode = 0;
	}
	Sync();
}

uint8 MapperNum = 32;
} // namespace

const MapperInfo MapperInfo_032 =
{
	&MapperNum,
	_T("Irem G-101"),
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
