/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_MMC4.h"

typedef	struct	MMC4
{
	u8 PRG;
	u8 LatchState[2];
	u8 Latch0[2];
	u8 Latch1[2];
	u8 Mirror;
	FSync Sync;
	FPPURead PPURead3, PPURead7;
}	TMMC4, *PMMC4;
static	TMMC4	MMC4;

void	MMC4_Load (FSync Sync)
{
	MMC4.Sync = Sync;
}

void	MMC4_Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 2; x++)
		{
			MMC4.LatchState[x] = 0;
			MMC4.Latch0[x] = 0;
			MMC4.Latch1[x] = 0;
		}
		MMC4.PRG = 0;
		MMC4.Mirror = 0;
	}
	EMU->SetCPUWriteHandler(0xA,MMC4_CPUWriteA);
	EMU->SetCPUWriteHandler(0xB,MMC4_CPUWriteB);
	EMU->SetCPUWriteHandler(0xC,MMC4_CPUWriteC);
	EMU->SetCPUWriteHandler(0xD,MMC4_CPUWriteD);
	EMU->SetCPUWriteHandler(0xE,MMC4_CPUWriteE);
	EMU->SetCPUWriteHandler(0xF,MMC4_CPUWriteF);
	MMC4.PPURead3 = EMU->GetPPUReadHandler(0x3);
	EMU->SetPPUReadHandler(0x3,MMC4_PPURead3);
	MMC4.PPURead7 = EMU->GetPPUReadHandler(0x7);
	EMU->SetPPUReadHandler(0x7,MMC4_PPURead7);
	MMC4.Sync();
}

void	MMC4_Unload (void)
{
}

int	MAPINT	MMC4_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,MMC4.Latch0[0]);
	SAVELOAD_BYTE(mode,x,data,MMC4.Latch0[1]);
	SAVELOAD_BYTE(mode,x,data,MMC4.Latch1[0]);
	SAVELOAD_BYTE(mode,x,data,MMC4.Latch1[1]);
	SAVELOAD_BYTE(mode,x,data,MMC4.LatchState[0]);
	SAVELOAD_BYTE(mode,x,data,MMC4.LatchState[1]);
	SAVELOAD_BYTE(mode,x,data,MMC4.PRG);
	SAVELOAD_BYTE(mode,x,data,MMC4.Mirror);
	if (mode == STATE_LOAD)
		MMC4.Sync();
	return x;
}

void	MMC4_SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM16(0x8,(MMC4.PRG & AND) | OR);
	EMU->SetPRG_ROM16(0xC,(0xF & AND) | OR);
}

void	MMC4_SyncCHR (void)
{
	EMU->SetCHR_ROM4(0,MMC4.Latch0[MMC4.LatchState[0]]);
	EMU->SetCHR_ROM4(4,MMC4.Latch1[MMC4.LatchState[1]]);
}

void	MMC4_SyncMirror (void)
{
	if (MMC4.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	MMC4_PPURead3 (int Bank, int Addr)
{
	int result = MMC4.PPURead3(Bank,Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		MMC4.LatchState[0] = 0;
		EMU->SetCHR_ROM4(0,MMC4.Latch0[0]);
	}
	else if (addy == 0x3E8)
	{
		MMC4.LatchState[0] = 1;
		EMU->SetCHR_ROM4(0,MMC4.Latch0[1]);
	}
	return result;
}

int	MAPINT	MMC4_PPURead7 (int Bank, int Addr)
{
	int result = MMC4.PPURead7(Bank,Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		MMC4.LatchState[1] = 0;
		EMU->SetCHR_ROM4(4,MMC4.Latch1[0]);
	}
	else if (addy == 0x3E8)
	{
		MMC4.LatchState[1] = 1;
		EMU->SetCHR_ROM4(4,MMC4.Latch1[1]);
	}
	return result;
}

void	MAPINT	MMC4_CPUWriteA (int Bank, int Addr, int Val)
{
	MMC4.PRG = Val & 0xF;
	MMC4.Sync();
}

void	MAPINT	MMC4_CPUWriteB (int Bank, int Addr, int Val)
{
	MMC4.Latch0[0] = Val & 0x1F;
	MMC4.Sync();
}

void	MAPINT	MMC4_CPUWriteC (int Bank, int Addr, int Val)
{
	MMC4.Latch0[1] = Val & 0x1F;
	MMC4.Sync();
}

void	MAPINT	MMC4_CPUWriteD (int Bank, int Addr, int Val)
{
	MMC4.Latch1[0] = Val & 0x1F;
	MMC4.Sync();
}

void	MAPINT	MMC4_CPUWriteE (int Bank, int Addr, int Val)
{
	MMC4.Latch1[1] = Val & 0x1F;
	MMC4.Sync();
}

void	MAPINT	MMC4_CPUWriteF (int Bank, int Addr, int Val)
{
	MMC4.Mirror = Val & 0x01;
	MMC4.Sync();
}
