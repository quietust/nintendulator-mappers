/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPUWrite _Write4;
u8 IRQenabled;
u16_n IRQcounter;
u8 PRG;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, 0xF);
	EMU->SetPRG_ROM8(0x8, 0x8);
	EMU->SetPRG_ROM8(0xA, 0x9);
	EMU->SetPRG_ROM8(0xC, ((PRG & 0x1) << 2) | ((PRG & 0x6) >> 1) | (PRG & 0x8));
	EMU->SetPRG_ROM8(0xE, 0xB);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		IRQcounter.s0++;
		if (IRQcounter.s0 >= 4096)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if ((Addr & 0x060) != 0x020)
		return;
	if (Addr & 0x100)
	{
		if (Val & 1)
			IRQenabled = 1;
		else
		{
			IRQcounter.s0 = 0;
			IRQenabled = 0;
			EMU->SetIRQ(1);
		}
	}
	else
	{
		PRG = Val;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		IRQenabled = 0;
		IRQcounter.s0 = 0;
	}
	Sync();
}

u8 MapperNum = 50;
} // namespace

CTMapperInfo	MapperInfo_050 =
{
	&MapperNum,
	_T("SMB2j rev. A"),
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
