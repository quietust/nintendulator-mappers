/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Mirror, VROM_use;
uint8 CHR_L, CHR_H;
uint8 PRG, CHR[4];

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	for (int x = 0; x < 4; x++)
		EMU->SetCHR_ROM2(x << 1, CHR[x]);
}

void	SyncNametables (void)
{
	switch (Mirror & 0x3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (VROM_use)
	{
		uint8 A = EMU->GetCHR_NT1(0x8) ? CHR_H : CHR_L;
		uint8 B = EMU->GetCHR_NT1(0x9) ? CHR_H : CHR_L;
		uint8 C = EMU->GetCHR_NT1(0xA) ? CHR_H : CHR_L;
		uint8 D = EMU->GetCHR_NT1(0xB) ? CHR_H : CHR_L;
		EMU->SetCHR_ROM1(0x8, A | 0x80);
		EMU->SetCHR_ROM1(0x9, B | 0x80);
		EMU->SetCHR_ROM1(0xA, C | 0x80);
		EMU->SetCHR_ROM1(0xB, D | 0x80);
		EMU->SetCHR_ROM1(0xC, A | 0x80);
		EMU->SetCHR_ROM1(0xD, B | 0x80);
		EMU->SetCHR_ROM1(0xE, C | 0x80);
		EMU->SetCHR_ROM1(0xF, D | 0x80);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Mirror);
	SAVELOAD_BYTE(mode, x, data, VROM_use);
	SAVELOAD_BYTE(mode, x, data, CHR_L);
	SAVELOAD_BYTE(mode, x, data, CHR_H);
	SAVELOAD_BYTE(mode, x, data, PRG);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	CHR[0] = Val;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	CHR[1] = Val;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	CHR[2] = Val;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	CHR[3] = Val;
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	CHR_L = Val;
	SyncNametables();
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	CHR_H = Val;
	SyncNametables();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mirror = Val & 0x3;
	VROM_use = (Val & 0x10);
	SyncNametables();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	PRG = Val;
	Sync();
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		for (int x = 0; x < 4; x++)
			CHR[x] = x;
		Mirror = VROM_use = 0;
		CHR_L = CHR_H = 0;
	}

	Sync();
	SyncNametables();
}

uint8 MapperNum = 68;
} // namespace

const MapperInfo MapperInfo_068 =
{
	&MapperNum,
	_T("SUNSOFT-4"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};