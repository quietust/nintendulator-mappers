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
uint8 CHR[4];
uint8 IRQenabled;
uint8 IRQcounter;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM2(0, CHR[0]);
	EMU->SetCHR_ROM2(2, CHR[1]);
	EMU->SetCHR_ROM2(4, CHR[2]);
	EMU->SetCHR_ROM2(6, CHR[3]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	uint8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_BYTE(mode, x, data, IRQcounter);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((IRQenabled) && (Cycle == 0) && IsRendering)
	{
		IRQcounter++;
		if (IRQcounter >= 8)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	CHR[Addr & 3] = Val;
	Sync();
}

void	MAPINT	Write7 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	PRG[0] = Val & 0xF;	break;
	case 1:	PRG[1] = Val & 0xF;	break;
	case 2:	IRQenabled = 0;
		IRQcounter = 0;
		EMU->SetIRQ(1);			break;
	case 3:	IRQenabled = 1;		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6, Write6);
	EMU->SetCPUWriteHandler(0x7, Write7);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = PRG[1] = 0;
		CHR[0] = CHR[1] = CHR[2] = CHR[3] = 0;
		IRQenabled = 0;
		IRQcounter = 0;
		EMU->SetIRQ(1);
	}
	Sync();
}

uint8 MapperNum = 91;
} // namespace

const MapperInfo MapperInfo_091 =
{
	&MapperNum,
	_T("PC-HK-SF3"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};