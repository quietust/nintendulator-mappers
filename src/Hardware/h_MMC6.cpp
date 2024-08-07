/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_MMC6.h"

namespace MMC6
{
uint8_t IRQenabled, IRQcounter, IRQlatch, IRQreload;
uint8_t IRQaddr;
uint8_t Cmd;
uint8_t PRG[4];
uint8_t CHR[8];
uint8_t WRAMEnab;
uint8_t Mirror;
FCPURead _CPURead7;
FCPUWrite _CPUWrite7;
FSync Sync;
BOOL SyncOnLoad;

void	Load (FSync _Sync, BOOL _SyncOnLoad)
{
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0x00;	PRG[1] = 0x01;	PRG[2] = 0x3E;	PRG[3] = 0x3F;	// 2 and 3 never change, simplifies GetPRGBank()

		CHR[0] = 0x00;	CHR[1] = 0x01;	CHR[2] = 0x02;	CHR[3] = 0x03;
		CHR[4] = 0x04;	CHR[5] = 0x05;	CHR[6] = 0x06;	CHR[7] = 0x07;

		IRQenabled = IRQcounter = IRQlatch = 0;
		Cmd = 0;
		WRAMEnab = 0;
		Mirror = 0;
	}
	EMU->SetPRG_RAM4(0x7, 0);
	_CPURead7 = EMU->GetCPUReadHandler(0x7);
	EMU->SetCPUReadHandler(0x7, CPURead7);
	EMU->SetCPUReadHandlerDebug(0x7, CPURead7);
	_CPUWrite7 = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x7, CPUWrite7);
	EMU->SetCPUWriteHandler(0x8, CPUWrite89);
	EMU->SetCPUWriteHandler(0x9, CPUWrite89);
	EMU->SetCPUWriteHandler(0xA, CPUWriteAB);
	EMU->SetCPUWriteHandler(0xB, CPUWriteAB);
	EMU->SetCPUWriteHandler(0xC, CPUWriteCD);
	EMU->SetCPUWriteHandler(0xD, CPUWriteCD);
	EMU->SetCPUWriteHandler(0xE, CPUWriteEF);
	EMU->SetCPUWriteHandler(0xF, CPUWriteEF);
	Sync();
}

void	Unload (void)
{
}

void	SyncMirror (void)
{
	if (Mirror & 1)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	GetPRGBank (int Bank)
{
	if (Bank & 0x1)
		return PRG[Bank];
	else	return PRG[Bank ^ ((Cmd & 0x40) >> 5)];
}

int	GetCHRBank (int Bank)
{
	return CHR[Bank ^ ((Cmd & 0x80) >> 5)];
}

void	SyncPRG (int AND, int OR)
{
	for (int i = 0; i < 4; i++)
		EMU->SetPRG_ROM8(0x8 | (i << 1), (GetPRGBank(i) & AND) | OR);
}

void	SyncCHR_ROM (int AND, int OR)
{
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, (GetCHRBank(i) & AND) | OR);
}

void	SyncCHR_RAM (int AND, int OR)
{
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_RAM1(i, (GetCHRBank(i) & AND) | OR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	SAVELOAD_BYTE(mode, offset, data, CHR[0]);
	SAVELOAD_BYTE(mode, offset, data, CHR[2]);
	if (IsLoad(mode))
	{
		CHR[1] = CHR[0] | 1;
		CHR[3] = CHR[2] | 1;
	}
	for (int i = 4; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, WRAMEnab);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	SAVELOAD_BYTE(mode, offset, data, IRQreload);
	SAVELOAD_BYTE(mode, offset, data, IRQaddr);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

int	MAPINT	CPURead7 (int Bank, int Addr)
{
	if (WRAMEnab & 0xA0)
	{
		if (WRAMEnab & ((Addr & 0x200) ? 0x80 : 0x20))
			return _CPURead7(0x7, Addr & 0x3FF);
		else	return 0;
	}
	else	return -1;
}

void	MAPINT	CPUWrite7 (int Bank, int Addr, int Val)
{
	if ((Addr & 0x200) ? ((WRAMEnab & 0xC0) == 0xC0) : ((WRAMEnab & 0x30) == 0x30))
		_CPUWrite7(0x7, Addr & 0x3FF, Val);
}

void	MAPINT	CPUWrite89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (Cmd & 0x7)
		{
		case 0:	CHR[0] = (Val & 0xFE) | 0;
			CHR[1] = (Val & 0xFE) | 1;break;
		case 1:	CHR[2] = (Val & 0xFE) | 0;
			CHR[3] = (Val & 0xFE) | 1;break;
		case 2:	CHR[4] = Val;		break;
		case 3:	CHR[5] = Val;		break;
		case 4:	CHR[6] = Val;		break;
		case 5:	CHR[7] = Val;		break;
		case 6:	PRG[0] = Val & 0x3F;	break;
		case 7:	PRG[1] = Val & 0x3F;	break;
		}
	else
	{
		Cmd = Val;
		if (!(Cmd & 0x20))
			WRAMEnab = 0;
	}
	Sync();
}

void	MAPINT	CPUWriteAB (int Bank, int Addr, int Val)
{
	if (Addr & 1)
	{
		if (Cmd & 0x20)
			WRAMEnab = Val;
	}
	else	Mirror = Val;
	Sync();
}

void	MAPINT	CPUWriteCD (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		IRQcounter = 0;
	else	IRQlatch = Val;
}

void	MAPINT	CPUWriteEF (int Bank, int Addr, int Val)
{
	IRQenabled = (Addr & 1);
	if (!IRQenabled)
		EMU->SetIRQ(1);
}
void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (IRQaddr)
		IRQaddr--;
	if ((!IRQaddr) && (Addr & 0x1000))
	{
		if (!IRQcounter || IRQreload)
			IRQcounter = IRQlatch;
		else	IRQcounter--;
		if (!IRQcounter && IRQenabled)
			EMU->SetIRQ(0);
	}
	if (Addr & 0x1000)
		IRQaddr = 8;
}
} // namespace MMC6
