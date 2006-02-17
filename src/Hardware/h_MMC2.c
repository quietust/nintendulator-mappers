#include	"h_MMC2.h"

static	TMMC2	MMC2;

void	MMC2_Init (RESET_TYPE ResetType, FSync Sync)
{
	u8 x;
	for (x = 0; x < 2; x++)
	{
		MMC2.LatchState[x] = 0;
		MMC2.Latch0[x] = 0;
		MMC2.Latch1[x] = 0;
	}
	MMC2.PRG = 0;
	MMC2.Mirror = 0;
	EMU->SetCPUWriteHandler(0xA,MMC2_CPUWriteA);
	EMU->SetCPUWriteHandler(0xB,MMC2_CPUWriteB);
	EMU->SetCPUWriteHandler(0xC,MMC2_CPUWriteC);
	EMU->SetCPUWriteHandler(0xD,MMC2_CPUWriteD);
	EMU->SetCPUWriteHandler(0xE,MMC2_CPUWriteE);
	EMU->SetCPUWriteHandler(0xF,MMC2_CPUWriteF);
	MMC2.PPURead3 = EMU->GetPPUReadHandler(0x3);
	EMU->SetPPUReadHandler(0x3,MMC2_PPURead3);
	MMC2.PPURead7 = EMU->GetPPUReadHandler(0x7);
	EMU->SetPPUReadHandler(0x7,MMC2_PPURead7);
	(MMC2.Sync = Sync)();
}

void	MMC2_Destroy (void)
{
	EMU->SetPPUReadHandler(0x3,MMC2.PPURead3);
	EMU->SetPPUReadHandler(0x7,MMC2.PPURead7);
}

int	_MAPINT	MMC2_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,MMC2.Latch0[0])
	SAVELOAD_BYTE(mode,x,data,MMC2.Latch0[1])
	SAVELOAD_BYTE(mode,x,data,MMC2.Latch1[0])
	SAVELOAD_BYTE(mode,x,data,MMC2.Latch1[1])
	SAVELOAD_BYTE(mode,x,data,MMC2.LatchState[0])
	SAVELOAD_BYTE(mode,x,data,MMC2.LatchState[1])
	SAVELOAD_BYTE(mode,x,data,MMC2.PRG)
	SAVELOAD_BYTE(mode,x,data,MMC2.Mirror)
	if (mode == STATE_LOAD)
		MMC2.Sync();
	return x;
}

void	MMC2_SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM8(0x8,(MMC2.PRG & AND) | OR);
	EMU->SetPRG_ROM8(0xA,(0xD & AND) | OR);
	EMU->SetPRG_ROM8(0xC,(0xE & AND) | OR);
	EMU->SetPRG_ROM8(0xE,(0xF & AND) | OR);
}

void	MMC2_SyncCHR (void)
{
	EMU->SetCHR_ROM4(0,MMC2.Latch0[MMC2.LatchState[0]]);
	EMU->SetCHR_ROM4(4,MMC2.Latch1[MMC2.LatchState[1]]);
}

void	MMC2_SyncMirror (void)
{
	if (MMC2.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	_MAPINT	MMC2_PPURead3 (int Bank, int Addr)
{
	int result = MMC2.PPURead3(Bank,Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		MMC2.LatchState[0] = 0;
		EMU->SetCHR_ROM4(0,MMC2.Latch0[0]);
	}
	else if (addy == 0x3E8)
	{
		MMC2.LatchState[0] = 1;
		EMU->SetCHR_ROM4(0,MMC2.Latch0[1]);
	}
	return result;
}

int	_MAPINT	MMC2_PPURead7 (int Bank, int Addr)
{
	int result = MMC2.PPURead7(Bank,Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		MMC2.LatchState[1] = 0;
		EMU->SetCHR_ROM4(4,MMC2.Latch1[0]);
	}
	else if (addy == 0x3E8)
	{
		MMC2.LatchState[1] = 1;
		EMU->SetCHR_ROM4(4,MMC2.Latch1[1]);
	}
	return result;
}

void	_MAPINT	MMC2_CPUWriteA (int Bank, int Addr, int Val)
{
	MMC2.PRG = Val & 0xF;
	MMC2.Sync();
}

void	_MAPINT	MMC2_CPUWriteB (int Bank, int Addr, int Val)
{
	MMC2.Latch0[0] = Val & 0x1F;
	MMC2.Sync();
}

void	_MAPINT	MMC2_CPUWriteC (int Bank, int Addr, int Val)
{
	MMC2.Latch0[1] = Val & 0x1F;
	MMC2.Sync();
}

void	_MAPINT	MMC2_CPUWriteD (int Bank, int Addr, int Val)
{
	MMC2.Latch1[0] = Val & 0x1F;
	MMC2.Sync();
}

void	_MAPINT	MMC2_CPUWriteE (int Bank, int Addr, int Val)
{
	MMC2.Latch1[1] = Val & 0x1F;
	MMC2.Sync();
}

void	_MAPINT	MMC2_CPUWriteF (int Bank, int Addr, int Val)
{
	MMC2.Mirror = Val & 0x1;
	MMC2.Sync();
}
