/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t IRQenabled, IRQcounter, IRQlatch, IRQreload;
uint8_t IRQmode, IRQaddr, IRQcycle, IRQdelay;
uint8_t Cmd;
uint8_t PRG[3];
uint8_t CHR[8];
uint8_t Mirror;

void	Sync (void)
{
	uint8_t SwCHR = (Cmd & 0x80) >> 5;
	if (Mirror & 0x1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();

	if (Cmd & 0x40)
	{
		EMU->SetPRG_ROM8(0x8, PRG[2]);
		EMU->SetPRG_ROM8(0xC, PRG[0]);
	}
	else
	{
		EMU->SetPRG_ROM8(0x8, PRG[0]);
		EMU->SetPRG_ROM8(0xC, PRG[2]);
	}
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xE, -1);

	if (Cmd & 0x20)
	{
		for (int i = 0; i < 8; i++)
			EMU->SetCHR_ROM1(SwCHR ^ i, CHR[i]);
	}
	else
	{
		for (int i = 0; i < 4; i += 2)
			EMU->SetCHR_ROM2(SwCHR ^ i, CHR[i] >> 1);
		for (int i = 4; i < 8; i++)
			EMU->SetCHR_ROM1(SwCHR ^ i, CHR[i]);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (ver == 1)
	{
		SAVELOAD_BYTE(mode, offset, data, IRQreload);
		SAVELOAD_BYTE(mode, offset, data, IRQmode);
		SAVELOAD_BYTE(mode, offset, data, IRQaddr);
		SAVELOAD_BYTE(mode, offset, data, IRQcycle);
		SAVELOAD_BYTE(mode, offset, data, IRQdelay);
	}
	else
	{
		// ver != 1 can only occur during Load,
		// so initialize all new variables
		IRQreload = 0;
		IRQmode = 0;
		IRQaddr = 0;
		IRQcycle = 0;
		IRQdelay = 0;
	}

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	// NOTE: IsRendering check required to avoid flashing status bar in Hard Drivin'
	if (IRQmode || !IsRendering)
		return;

	if (IRQaddr)
		IRQaddr--;
	if (Addr & 0x1000)
	{
		if (!IRQaddr)
			IRQcycle = 1;
		IRQaddr = 8;
	}
}

void	MAPINT	CPUCycle (void)
{
	bool doClock = false;
	if (IRQmode)
	{
		if (!--IRQcycle)
		{
			doClock = true;
			IRQcycle = 4;
		}
	}
	else if (IRQcycle && !--IRQcycle)
		doClock = true;

	if (doClock)
	{
		if (IRQreload)
		{
			IRQreload = 0;
			IRQcounter = IRQlatch;
			if (IRQlatch)
				IRQcounter |= 1;
		}
		else if (!IRQcounter)
			IRQcounter = IRQlatch;
		else	IRQcounter--;

		if (IRQenabled && !IRQcounter)
			IRQdelay = 4;
	}

	if (IRQdelay && !--IRQdelay)
		EMU->SetIRQ(0);
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
		IRQcycle = IRQmode ? 4 : 0;
	}
	else	IRQlatch = Val;
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	IRQenabled = (Addr & 1);
	// NOTE: acknowledge on $E001 to avoid flickering scanline in Hard Drivin'
	// if (!IRQenabled)
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
		for (int i = 0; i < 3; i++)
			PRG[i] = 0xFF;
		for (int i = 0; i < 8; i++)
			CHR[i] = i;
		IRQenabled = IRQcounter = IRQlatch = IRQreload = 0;
		IRQmode = IRQaddr = IRQcycle = IRQdelay = 0;
		Cmd = 0;
		Mirror = 0;
	}

	Sync();
}

uint16_t MapperNum = 64;
} // namespace

const MapperInfo MapperInfo_064
(
	&MapperNum,
	_T("Tengen RAMBO-1"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
