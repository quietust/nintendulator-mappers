/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_MMC4.h"

namespace MMC4
{
uint8_t PRG;
uint8_t LatchState[2];
uint8_t Latch0[2];
uint8_t Latch1[2];
uint8_t Mirror;
FSync Sync;
BOOL SyncOnLoad;
FPPURead _PPURead3, _PPURead7;

void	Load (FSync _Sync, BOOL _SyncOnLoad)
{
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		for (int i = 0; i < 2; i++)
		{
			LatchState[i] = 0;
			Latch0[i] = 0;
			Latch1[i] = 0;
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
	EMU->SetPPUReadHandlerDebug(0x3, _PPURead3);
	_PPURead7 = EMU->GetPPUReadHandler(0x7);
	EMU->SetPPUReadHandler(0x7, PPURead7);
	EMU->SetPPUReadHandlerDebug(0x7, _PPURead7);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Latch0[0]);
	SAVELOAD_BYTE(mode, offset, data, Latch0[1]);
	SAVELOAD_BYTE(mode, offset, data, Latch1[0]);
	SAVELOAD_BYTE(mode, offset, data, Latch1[1]);
	SAVELOAD_BYTE(mode, offset, data, LatchState[0]);
	SAVELOAD_BYTE(mode, offset, data, LatchState[1]);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode) && SyncOnLoad)
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
	EMU->SetCHR_ROM4(0x0, Latch0[LatchState[0]]);
	EMU->SetCHR_ROM4(0x4, Latch1[LatchState[1]]);
}

void	SyncMirror (void)
{
	if (Mirror)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	PPURead3 (int Bank, int Addr)
{
	int result = _PPURead3(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		LatchState[0] = 0;
		EMU->SetCHR_ROM4(0x0, Latch0[0]);
	}
	else if (addy == 0x3E8)
	{
		LatchState[0] = 1;
		EMU->SetCHR_ROM4(0x0, Latch0[1]);
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
		EMU->SetCHR_ROM4(0x4, Latch1[0]);
	}
	else if (addy == 0x3E8)
	{
		LatchState[1] = 1;
		EMU->SetCHR_ROM4(0x4, Latch1[1]);
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
