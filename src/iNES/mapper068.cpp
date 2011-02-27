/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Mirror, VROM_use;
uint8 NT[2];
uint8 PRG, CHR[4];

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	for (int i = 0; i < 4; i++)
		EMU->SetCHR_ROM2(i << 1, CHR[i]);
}

void	SyncNametables (void)
{
	uint8 nt[4] = {0,0,0,0};
	void (MAPINT *SetCHR)(int,int) = EMU->SetCHR_NT1;

	switch (Mirror & 0x3)
	{
	case 0:	nt[0] = 0; nt[1] = 1; nt[2] = 0; nt[3] = 1;	break;	// V
	case 1:	nt[0] = 0; nt[1] = 0; nt[2] = 1; nt[3] = 1;	break;	// H
	case 2:	nt[0] = 0; nt[1] = 0; nt[2] = 0; nt[3] = 0;	break;	// S0
	case 3:	nt[0] = 1; nt[1] = 1; nt[2] = 1; nt[3] = 1;	break;	// S1
	}
	if (VROM_use)
	{
		SetCHR = EMU->SetCHR_ROM1;
		for (int i = 0; i < 4; i++)
			nt[i] = NT[nt[i]] | 0x80;
	}

	SetCHR(0x8, nt[0]);	SetCHR(0xC, nt[0]);
	SetCHR(0x9, nt[1]);	SetCHR(0xD, nt[1]);
	SetCHR(0xA, nt[2]);	SetCHR(0xE, nt[2]);
	SetCHR(0xB, nt[3]);	SetCHR(0xF, nt[3]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	SAVELOAD_BYTE(mode, offset, data, VROM_use);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, NT[i]);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
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
	NT[0] = Val;
	SyncNametables();
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	NT[1] = Val;
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

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
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
		for (int i = 0; i < 4; i++)
			CHR[i] = i;
		Mirror = VROM_use = 0;
		NT[0] = NT[1] = 0;
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