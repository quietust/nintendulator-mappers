/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_MMC1.h"

namespace MMC1
{
u8 Latch, LatchPos;
u8 Regs[4];
FSync Sync;

void	Load (FSync _Sync)
{
	Sync = _Sync;
}

void	Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ResetType == RESET_HARD)
	{
		Regs[0] = 0x0C;
		Regs[1] = 0x00;
		Regs[2] = 0x00;
		Regs[3] = 0x00;
		Latch = 0;
		LatchPos = 0;
	}
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x, Write);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, Regs[i]);
	SAVELOAD_BYTE(mode, x, data, Latch);
	SAVELOAD_BYTE(mode, x, data, LatchPos);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	LastReg;
void	MAPINT	Write (int Bank, int Addr, int Val)
{
	u8 Reg = (Bank >> 1) & 3;
	if (Val & 0x80)
	{
		Latch = LatchPos = 0;
		Regs[0] |= 0x0C;
		return;
	}
	// Ugly hack to get Bill & Ted's Excellent Video Game Adventure to run
	// since the mapper interface currently doesn't expose a CPU timestamp
	if (Reg != LastReg)
		Latch = LatchPos = 0;
	LastReg = Reg;
	Latch |= (Val & 1) << LatchPos++;
	if (LatchPos == 5)
	{
		Regs[Reg] = Latch & 0x1F;
		Latch = LatchPos = 0;
		Sync();
	}
}

void	SyncMirror (void)
{
	switch (Regs[0] & 0x3)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_V();	break;
	case 3:	EMU->Mirror_H();	break;
	}
}

int	GetPRGBankLo (void)
{
	if (Regs[0] & 0x08)
		if (Regs[0] & 0x04)
			return Regs[3] & 0xF;
		else	return 0x0;
	else	return (Regs[3] & 0xE) | 0;
}

int	GetPRGBankHi (void)
{
	if (Regs[0] & 0x08)
		if (Regs[0] & 0x04)
			return 0xF;
		else	return Regs[3] & 0xF;
	else	return (Regs[3] & 0xE) | 1;
}

int	GetCHRBankLo (void)
{
	if (Regs[0] & 0x10)
		return Regs[1] & 0x1F;
	else	return (Regs[1] & 0x1E) | 0;
}

int	GetCHRBankHi (void)
{
	if (Regs[0] & 0x10)
		return Regs[2] & 0x1F;
	else	return (Regs[1] & 0x1E) | 1;
}

BOOL	GetWRAMEnabled (void)
{
	return !(Regs[3] & 0x10);
}

void	SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM16(0x8, (GetPRGBankLo() & AND) | OR);
	EMU->SetPRG_ROM16(0xC, (GetPRGBankHi() & AND) | OR);
}

void	SyncCHR_ROM (int AND, int OR)
{
	EMU->SetCHR_ROM4(0, (GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_ROM4(4, (GetCHRBankHi() & AND) | OR);
}

void	SyncCHR_RAM (int AND, int OR)
{
	EMU->SetCHR_RAM4(0, (GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_RAM4(4, (GetCHRBankHi() & AND) | OR);
}

void	SyncWRAM (void)
{
	if (GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6, 0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}
} // namespace MMC1