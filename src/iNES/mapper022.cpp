/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRG[2];
u8_n CHR[8];
u8 Mirror;

void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, 0x1F);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x, CHR[x].b0 >> 1);
	if (Mirror & 0x1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i].b0);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val & 0x1F;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mirror = Val & 0xF;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0x1F;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 1:	CHR[1].n0 = Val & 0xF;	break;
	case 2:	CHR[0].n1 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[2].n0 = Val & 0xF;	break;
	case 1:	CHR[3].n0 = Val & 0xF;	break;
	case 2:	CHR[2].n1 = Val & 0xF;	break;
	case 3:	CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[4].n0 = Val & 0xF;	break;
	case 1:	CHR[5].n0 = Val & 0xF;	break;
	case 2:	CHR[4].n1 = Val & 0xF;	break;
	case 3:	CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[6].n0 = Val & 0xF;	break;
	case 1:	CHR[7].n0 = Val & 0xF;	break;
	case 2:	CHR[6].n1 = Val & 0xF;	break;
	case 3:	CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;	PRG[1] = 1;
		for (x = 0; x < 8; x++)
			CHR[x].b0 = x;
		Mirror = 0;
	}
	Sync();
}

u8 MapperNum = 22;
} // namespace

CTMapperInfo	MapperInfo_022 =
{
	&MapperNum,
	_T("Konami VRC2/VRC4"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};