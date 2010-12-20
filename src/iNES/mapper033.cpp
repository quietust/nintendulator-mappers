/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG[2];
uint8 CHR[6];
uint8 Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM2(0, CHR[0]);
	EMU->SetCHR_ROM2(2, CHR[1]);
	EMU->SetCHR_ROM1(4, CHR[2]);
	EMU->SetCHR_ROM1(5, CHR[3]);
	EMU->SetCHR_ROM1(6, CHR[4]);
	EMU->SetCHR_ROM1(7, CHR[5]);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	uint8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	PRG[0] = Val & 0x1F;
		Mirror = Val & 0x40;	break;
	case 1:	PRG[1] = Val & 0x1F;	break;
	case 2:	CHR[0] = Val;		break;
	case 3:	CHR[1] = Val;		break;
	}
	Sync();
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[2] = Val;	break;
	case 1:	CHR[3] = Val;	break;
	case 2:	CHR[4] = Val;	break;
	case 3:	CHR[5] = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	uint8 x;
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;	PRG[1] = 1;
		for (x = 0; x < 6; x++)
			CHR[x] = x;
		Mirror = 0;
	}
	Sync();
}

uint8 MapperNum = 33;
} // namespace

const MapperInfo MapperInfo_033 =
{
	&MapperNum,
	_T("Taito TC0190"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
