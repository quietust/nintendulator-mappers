/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_MMC4.h"

namespace MMC4
{
uint8 PRG;
uint8 LatchState[2];
uint8 Latch0[2];
uint8 Latch1[2];
uint8 Mirror;
FSync Sync;
FPPURead _PPURead3, _PPURead7;

void	Load (FSync _Sync)
{
	Sync = _Sync;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		for (int x = 0; x < 2; x++)
		{
			LatchState[x] = 0;
			Latch0[x] = 0;
			Latch1[x] = 0;
		}
		PRG = 0;
		Mirror = 0;
	}
	EMU->SetCPUWriteHandler(0xA, CPUWriteA);
	EMU->SetCPUWriteHandler(0xB, CPUWriteB);
	EMU->SetCPUWriteHandler(0xC, CPUWriteC);
	EMU->SetCPUWriteHandler(0xD, CPUWriteD);
	EMU->SetCPUWriteHandler(0xE, CPUWriteE);
	EMU->SetCPUWriteHandler(0xF, CPUWriteF);
	_PPURead3 = EMU->GetPPUReadHandler(0x3);
	EMU->SetPPUReadHandler(0x3, PPURead3);
	_PPURead7 = EMU->GetPPUReadHandler(0x7);
	EMU->SetPPUReadHandler(0x7, PPURead7);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Latch0[0]);
	SAVELOAD_BYTE(mode, offset, data, Latch0[1]);
	SAVELOAD_BYTE(mode, offset, data, Latch1[0]);
	SAVELOAD_BYTE(mode, offset, data, Latch1[1]);
	SAVELOAD_BYTE(mode, offset, data, LatchState[0]);
	SAVELOAD_BYTE(mode, offset, data, LatchState[1]);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	SyncPRG (int AND, int OR)
{
	EMU->SetPRG_ROM16(0x8, (PRG & AND) | OR);
	EMU->SetPRG_ROM16(0xC, (0xF & AND) | OR);
}

void	SyncCHR (void)
{
	EMU->SetCHR_ROM4(0, Latch0[LatchState[0]]);
	EMU->SetCHR_ROM4(4, Latch1[LatchState[1]]);
}

void	SyncMirror (void)
{
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	PPURead3 (int Bank, int Addr)
{
	int result = _PPURead3(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		LatchState[0] = 0;
		EMU->SetCHR_ROM4(0, Latch0[0]);
	}
	else if (addy == 0x3E8)
	{
		LatchState[0] = 1;
		EMU->SetCHR_ROM4(0, Latch0[1]);
	}
	return result;
}

int	MAPINT	PPURead7 (int Bank, int Addr)
{
	int result = _PPURead7(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		LatchState[1] = 0;
		EMU->SetCHR_ROM4(4, Latch1[0]);
	}
	else if (addy == 0x3E8)
	{
		LatchState[1] = 1;
		EMU->SetCHR_ROM4(4, Latch1[1]);
	}
	return result;
}

void	MAPINT	CPUWriteA (int Bank, int Addr, int Val)
{
	PRG = Val & 0xF;
	Sync();
}

void	MAPINT	CPUWriteB (int Bank, int Addr, int Val)
{
	Latch0[0] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWriteC (int Bank, int Addr, int Val)
{
	Latch0[1] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWriteD (int Bank, int Addr, int Val)
{
	Latch1[0] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWriteE (int Bank, int Addr, int Val)
{
	Latch1[1] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWriteF (int Bank, int Addr, int Val)
{
	Mirror = Val & 0x01;
	Sync();
}
} // namespace MMC4