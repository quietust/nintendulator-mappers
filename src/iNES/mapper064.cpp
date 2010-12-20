/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 IRQenabled, IRQcounter, IRQlatch, IRQmode, IRQreload;
uint16 IRQaddr;
uint8 Cmd;
uint8 PRG[3];
uint8 CHR[8];
uint8 Mirror;

void	Sync (void)
{
	uint8 SwCHR = (Cmd & 0x80) >> 5;
	if (Mirror & 0x1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();

	if (Cmd & 0x40)
	{
		EMU->SetPRG_ROM8(0xA, PRG[0]);
		EMU->SetPRG_ROM8(0xC, PRG[1]);
		EMU->SetPRG_ROM8(0x8, PRG[2]);
	}
	else
	{
		EMU->SetPRG_ROM8(0x8, PRG[0]);
		EMU->SetPRG_ROM8(0xA, PRG[1]);
		EMU->SetPRG_ROM8(0xC, PRG[2]);
	}
	EMU->SetPRG_ROM8(0xE, -1);

	if (Cmd & 0x20)
	{
		for (int x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(SwCHR ^ x, CHR[x]);
	}
	else
	{
		for (int x = 0; x < 4; x += 2)
			EMU->SetCHR_ROM2(SwCHR ^ x, CHR[x] >> 1);
		for (int x = 4; x < 8; x++)
			EMU->SetCHR_ROM1(SwCHR ^ x, CHR[x]);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	HBlank (void)
{
	if (!IRQcounter || IRQreload)
	{
		IRQcounter = IRQlatch + IRQreload;
		IRQreload = 0;
	}
	else if (!--IRQcounter)
	{
		if (IRQenabled)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (!(IRQmode) && (IsRendering) && (Cycle == 264))
		HBlank();
}

int cycles = 4;
void	MAPINT	CPUCycle (void)
{
	if ((IRQmode) && (!--cycles))
	{
		cycles = 4;
		HBlank();
	}
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (Cmd & 0xF)
		{
		case 0:	CHR[0] = Val;	break;
		case 1:	CHR[2] = Val;	break;
		case 2:	CHR[4] = Val;	break;
		case 3:	CHR[5] = Val;	break;
		case 4:	CHR[6] = Val;	break;
		case 5:	CHR[7] = Val;	break;
		case 6:	PRG[0] = Val;	break;
		case 7:	PRG[1] = Val;	break;

		case 8:	CHR[1] = Val;	break;
		case 9:	CHR[3] = Val;	break;
		case 15:PRG[2] = Val;	break;
		}
	else	Cmd = Val;
	Sync();
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		;
	else	Mirror = Val;
	Sync();
}

void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Addr & 1)
	{
		IRQmode = Val & 1;
		IRQreload = 1;
	}
	else	IRQlatch = Val;
}

void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	IRQenabled = (Addr & 1);
	if (!IRQenabled)
		EMU->SetIRQ(1);
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);

	if (ResetType == RESET_HARD)
	{
		for (int x = 0; x < 3; x++)
			PRG[x] = 0xFF;
		for (int x = 0; x < 8; x++)
			CHR[x] = x;
		IRQenabled = IRQcounter = IRQlatch = IRQmode = IRQreload = 0;
		IRQaddr = 0;
		Cmd = 0;
		Mirror = 0;
	}

	Sync();
}

uint8 MapperNum = 64;
} // namespace

const MapperInfo MapperInfo_064 =
{
	&MapperNum,
	_T("Tengen RAMBO-1"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};
