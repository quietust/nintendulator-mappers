#include	"h_MMC1.h"

static	TMMC1	MMC1;

void	MMC1_Load (FSync Sync)
{
	MMC1.Sync = Sync;
}

void	MMC1_Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ResetType == RESET_HARD)
	{
		MMC1.Regs[0] = 0x0C;
		MMC1.Regs[1] = 0x00;
		MMC1.Regs[2] = 0x00;
		MMC1.Regs[3] = 0x00;
		MMC1.Latch = 0;
		MMC1.LatchPos = 0;
	}
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,MMC1_Write);
	MMC1.Sync();
}

void	MMC1_Unload (void)
{
}

int	_MAPINT	MMC1_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,MMC1.Regs[i])
	SAVELOAD_BYTE(mode,x,data,MMC1.Latch)
	SAVELOAD_BYTE(mode,x,data,MMC1.LatchPos)
	if (mode == STATE_LOAD)
		MMC1.Sync();
	return x;
}

static	int	LastReg;
void	_MAPINT	MMC1_Write (int Bank, int Addr, int Val)
{
	u8 Reg = (Bank >> 1) & 3;
	if (Val & 0x80)
	{
		MMC1.Latch = MMC1.LatchPos = 0;
		MMC1.Regs[0] |= 0x0C;
		return;
	}
	if (Reg != LastReg)
		MMC1.Latch = MMC1.LatchPos = 0;
	LastReg = Reg;
	MMC1.Latch |= (Val & 1) << MMC1.LatchPos++;
	if (MMC1.LatchPos == 5)
	{
		MMC1.Regs[Reg] = MMC1.Latch & 0x1F;
		MMC1.Latch = MMC1.LatchPos = 0;
		MMC1.Sync();
	}
}

void	MMC1_SyncMirror (void)
{
	switch (MMC1.Regs[0] & 0x3)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_V();	break;
	case 3:	EMU->Mirror_H();	break;
	}
}

int	MMC1_GetPRGBankLo (void)
{
	if (MMC1.Regs[0] & 0x08)
		if (MMC1.Regs[0] & 0x04)
			return MMC1.Regs[3] & 0xF;
		else	return 0x0;
	else	return (MMC1.Regs[3] & 0xE) | 0;
}

int	MMC1_GetPRGBankHi (void)
{
	if (MMC1.Regs[0] & 0x08)
		if (MMC1.Regs[0] & 0x04)
			return 0xF;
		else	return MMC1.Regs[3] & 0xF;
	else	return (MMC1.Regs[3] & 0xE) | 1;
}

int	MMC1_GetCHRBankLo (void)
{
	if (MMC1.Regs[0] & 0x10)
		return MMC1.Regs[1] & 0x1F;
	else	return (MMC1.Regs[1] & 0x1E) | 0;
}

int	MMC1_GetCHRBankHi (void)
{
	if (MMC1.Regs[0] & 0x10)
		return MMC1.Regs[2] & 0x1F;
	else	return (MMC1.Regs[1] & 0x1E) | 1;
}

BOOL	MMC1_GetWRAMEnabled (void)
{
	return !(MMC1.Regs[3] & 0x10);
}

void	MMC1_SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM16(0x8,(MMC1_GetPRGBankLo() & AND) | OR);
	EMU->SetPRG_ROM16(0xC,(MMC1_GetPRGBankHi() & AND) | OR);
}

void	MMC1_SyncCHR_ROM (int AND, int OR)
{
	EMU->SetCHR_ROM4(0,(MMC1_GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_ROM4(4,(MMC1_GetCHRBankHi() & AND) | OR);
}

void	MMC1_SyncCHR_RAM (int AND, int OR)
{
	EMU->SetCHR_RAM4(0,(MMC1_GetCHRBankLo() & AND) | OR);
	EMU->SetCHR_RAM4(4,(MMC1_GetCHRBankHi() & AND) | OR);
}

void	MMC1_SyncWRAM (void)
{
	if (MMC1_GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6,0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}