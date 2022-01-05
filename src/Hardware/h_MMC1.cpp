/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_MMC1.h"

namespace MMC1
{
uint8_t Latch, LatchPos;
uint8_t Regs[4];
FSync Sync;
BOOL SyncOnLoad;
BOOL IsMMC1A;

void	Load (FSync _Sync, BOOL _SyncOnLoad, BOOL _IsMMC1A)
{
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
	IsMMC1A = _IsMMC1A;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		Regs[0] = 0x0C;
		Regs[1] = 0x00;
		Regs[2] = 0x00;
		Regs[3] = 0x00;
		Latch = 0;
		LatchPos = 0;
	}
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, Regs[i]);
	SAVELOAD_BYTE(mode, offset, data, Latch);
	SAVELOAD_BYTE(mode, offset, data, LatchPos);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

int	LastReg;
void	MAPINT	Write (int Bank, int Addr, int Val)
{
	uint8_t Reg = (Bank >> 1) & 3;
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
	int bank;
	if (Regs[0] & 0x08)
		if (Regs[0] & 0x04)
			bank = Regs[3] & 0xF;
		else	bank = 0x0;
	else	bank = (Regs[3] & 0xE) | 0;
	if (IsMMC1A && (Regs[3] & 0x10))
		bank = (bank & 0x7) | (Regs[3] & 0x8);
	return bank;
}

int	GetPRGBankHi (void)
{
	int bank;
	if (Regs[0] & 0x08)
		if (Regs[0] & 0x04)
			bank = 0xF;
		else	bank = Regs[3] & 0xF;
	else	bank = (Regs[3] & 0xE) | 1;
	if (IsMMC1A && (Regs[3] & 0x10))
		bank = (bank & 0x7) | (Regs[3] & 0x8);
	return bank;
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
	if (IsMMC1A)
		return TRUE;
	return !(Regs[3] & 0x10);
}

void	SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM16(0x8, (GetPRGBankLo() & AND) | OR);
	EMU->SetPRG_ROM16(0xC, (GetPRGBankHi() & AND) | OR);
}

void	SyncCHR_ROM (int AND, int OR)
{
	EMU->SetCHR_ROM4(0x0, (GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_ROM4(0x4, (GetCHRBankHi() & AND) | OR);
}

void	SyncCHR_RAM (int AND, int OR)
{
	EMU->SetCHR_RAM4(0x0, (GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_RAM4(0x4, (GetCHRBankHi() & AND) | OR);
}

void	SyncWRAM (void)
{
	int bank = 0;
	if (IsMMC1A && (Regs[3] & 0x10))
		bank = (Regs[3] & 0x8) >> 3;
	if (GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6, bank);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}
} // namespace MMC1
