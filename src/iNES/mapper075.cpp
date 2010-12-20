/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRG[3];
u8_n CHR[2];
u8 Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, 0xF);
	EMU->SetCHR_ROM4(0, CHR[0].b0);
	EMU->SetCHR_ROM4(4, CHR[1].b0);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i].b0);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val & 0xF;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mirror = Val & 0x1;
	CHR[0].n1 = (Val & 0x2) >> 1;
	CHR[1].n1 = (Val & 0x4) >> 2;
	Sync();
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0xF;
	Sync();
}

void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	PRG[2] = Val & 0xF;
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	CHR[0].n0 = Val & 0xF;
	Sync();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	CHR[1].n0 = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		CHR[0].b0 = CHR[1].b0 = 0;
		PRG[0] = PRG[1] = PRG[2] = 0;
		Mirror = 0;
	}

	Sync();
}

u8 MapperNum = 75;
} // namespace

const MapperInfo MapperInfo_075 =
{
	&MapperNum,
	_T("Konami VRC1"),
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