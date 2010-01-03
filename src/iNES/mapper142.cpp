/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRGcontrol;
u8 PRG[4];
u16_n IRQcounter;
u8 IRQenabled;

void	Sync (void)
{
	u8 x;
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1), PRG[x]);
	EMU->SetPRG_ROM8(0x6, PRG[3]);
	EMU->SetPRG_ROM8(0xE, -1);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode, x, data, PRGcontrol);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		if (IRQcounter.s0 == 0xFFFF)
		{
			IRQenabled = 0;
			EMU->SetIRQ(0);
		}
		else	IRQcounter.s0++;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	IRQcounter.n0 = Val & 0xF;
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	IRQcounter.n1 = Val & 0xF;
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	IRQcounter.n2 = Val & 0xF;
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	IRQcounter.n3 = Val & 0xF;
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	IRQenabled = Val & 0xF;
	EMU->SetIRQ(1);
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	PRGcontrol = Val & 0xF;
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (PRGcontrol)
	{
	case 0x1:	PRG[0] = Val & 0xF;	break;
	case 0x2:	PRG[1] = Val & 0xF;	break;
	case 0x3:	PRG[2] = Val & 0xF;	break;
	case 0x4:	PRG[3] = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			PRG[x] = 0;
		PRGcontrol = 0;
	}

	Sync();
}

u8 MapperNum = 142;
} // namespace

CTMapperInfo	MapperInfo_142 =
{
	&MapperNum,
	_T("SMB2j Pirate (KS 202)"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};