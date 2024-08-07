/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\resource.h"

namespace DripSound
{
struct
{
	uint8_t FIFO[256], ReadPos, WritePos;
	BOOL IsFull, IsEmpty;
	int freq, vol;
	int timer;
	int Pos;

	int	GenerateWave (int Cycles)
	{
		static int phase = 0;
		int z = 0;
		for (int i = 0; i < Cycles; i++)
		{
			phase = (phase + 1) & 0xF;
			if (IsEmpty)
				continue;
			if (!timer--)
			{
				timer = freq;
				if (ReadPos == WritePos)
					IsFull = FALSE;
				Pos = FIFO[++ReadPos] - 0x80;
				if (ReadPos == WritePos)
					IsEmpty = TRUE;
			}
			if (vol >= phase)
				z += Pos << 4;
		}
		return z / Cycles;
	}
	int Read (void)
	{
		int result = 0;
		if (IsFull)
			result |= 0x80;
		if (IsEmpty)
			result |= 0x40;
		return result;
	}
	void Write (int Addr, int Val)
	{
		switch (Addr)
		{
		case 0x0:
			ZeroMemory(FIFO, 256);
			ReadPos = WritePos = 0;
			IsFull = FALSE;
			IsEmpty = TRUE;
			Pos = 0;
			timer = freq;
			break;
		case 0x1:
			if (ReadPos == WritePos)
			{
				IsEmpty = FALSE;
				Pos = Val - 0x80;
				timer = freq;
			}
			FIFO[WritePos++] = Val;
			if (ReadPos == WritePos)
				IsFull = TRUE;
			break;
		case 0x2:
			freq = (freq & 0xF00) | Val;
			break;
		case 0x3:
			freq = (freq & 0xFF) | ((Val & 0xF) << 8);
			vol = (Val & 0xF0) >> 4;
			if (!IsEmpty)
				Pos = FIFO[ReadPos] - 0x80;
			break;
		}
	}
} Chan[2];

int	MAPINT	MapperSnd (int Cycles)
{
	int out = 0;
	out += Chan[0].GenerateWave(Cycles);
	out += Chan[1].GenerateWave(Cycles);
	return out << 3;
}
int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode, offset, data, Chan[0].FIFO[i]);
	for (int i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode, offset, data, Chan[1].FIFO[i]);
	SAVELOAD_BYTE(mode, offset, data, Chan[0].ReadPos);
	SAVELOAD_BYTE(mode, offset, data, Chan[1].ReadPos);
	SAVELOAD_BYTE(mode, offset, data, Chan[0].WritePos);
	SAVELOAD_BYTE(mode, offset, data, Chan[1].WritePos);
	SAVELOAD_BYTE(mode, offset, data, Chan[0].IsFull);
	SAVELOAD_BYTE(mode, offset, data, Chan[1].IsFull);
	SAVELOAD_BYTE(mode, offset, data, Chan[0].IsEmpty);
	SAVELOAD_BYTE(mode, offset, data, Chan[1].IsEmpty);
	SAVELOAD_WORD(mode, offset, data, Chan[0].freq);
	SAVELOAD_WORD(mode, offset, data, Chan[1].freq);
	SAVELOAD_BYTE(mode, offset, data, Chan[0].vol);
	SAVELOAD_BYTE(mode, offset, data, Chan[1].vol);
	SAVELOAD_WORD(mode, offset, data, Chan[0].timer);
	SAVELOAD_WORD(mode, offset, data, Chan[1].timer);
	return offset;
}
} // namespace Sound

namespace
{
uint8_t PRG, CHR[4], Flags;
uint8_t IRQenabled, IRQlatch;
uint16_t IRQcounter;
uint8_t *ExtRam0, *ExtRam1;
uint16_t LastAddr, LastAddrTmp;
uint8_t Jumper;
FPPURead _PPUReadNT[4];
FCPURead _CPURead4;
HWND ConfigWindow;
uint8_t ConfigCmd;

int	MAPINT	PPUReadNT (int Bank, int Addr)
{
	if ((Flags & 0x4) && (Addr >= 0x3C0) && ((Bank & 3) == ((LastAddr >> 10) & 3)))
	{
		static const unsigned char AttribBits[4] = {0x00,0x55,0xAA,0xFF};
		if (EMU->GetCHR_NT1(Bank))
			return AttribBits[ExtRam1[LastAddr & 0x3FF]];
		else	return AttribBits[ExtRam0[LastAddr & 0x3FF]];
	}
	else	return _PPUReadNT[Bank & 3](Bank, Addr);
}

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, 0xF);
	EMU->SetCHR_ROM2(0x0, CHR[0]);
	EMU->SetCHR_ROM2(0x2, CHR[1]);
	EMU->SetCHR_ROM2(0x4, CHR[2]);
	EMU->SetCHR_ROM2(0x6, CHR[3]);
	switch (Flags & 0x3)
	{
	case 0:	EMU->Mirror_A10();	break;
	case 1:	EMU->Mirror_A11();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (Flags & 0x04)
	{
		EMU->SetPPUReadHandler(0x8, PPUReadNT);
		EMU->SetPPUReadHandler(0x9, PPUReadNT);
		EMU->SetPPUReadHandler(0xA, PPUReadNT);
		EMU->SetPPUReadHandler(0xB, PPUReadNT);
		EMU->SetPPUReadHandlerDebug(0x8, _PPUReadNT[0]);
		EMU->SetPPUReadHandlerDebug(0x9, _PPUReadNT[1]);
		EMU->SetPPUReadHandlerDebug(0xA, _PPUReadNT[2]);
		EMU->SetPPUReadHandlerDebug(0xB, _PPUReadNT[3]);
	}
	else
	{
		EMU->SetPPUReadHandler(0x8, _PPUReadNT[0]);
		EMU->SetPPUReadHandler(0x9, _PPUReadNT[1]);
		EMU->SetPPUReadHandler(0xA, _PPUReadNT[2]);
		EMU->SetPPUReadHandler(0xB, _PPUReadNT[3]);
	}
	if (Flags & 0x8)
		EMU->SetPRG_RAM8(0x6, 0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = DripSound::SaveLoad(mode, offset, data));
	SAVELOAD_WORD(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Flags);
	SAVELOAD_WORD(mode, offset, data, LastAddr);
	SAVELOAD_WORD(mode, offset, data, LastAddrTmp);
	SAVELOAD_BYTE(mode, offset, data, Jumper);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled) && (!--IRQcounter))
		EMU->SetIRQ(0);
}
void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	LastAddr = LastAddrTmp;
	LastAddrTmp = Addr;
}

int	MAPINT	CPURead4 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return Jumper | 'd';
	else	return _CPURead4(Bank, Addr);
}
int	MAPINT	CPURead5 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return DripSound::Chan[1].Read();
	else	return DripSound::Chan[0].Read();
}
void	MAPINT	CPUWriteL (int Bank, int Addr, int Val)
{
	if (Addr & 0x8)
	{
		switch (Addr & 0x7)
		{
		case 0:	IRQlatch = Val;
			break;
		case 1:	IRQcounter = ((Val & 0x7F) << 8) | IRQlatch;
			IRQenabled = Val & 0x80;
			EMU->SetIRQ(1);
			break;
		case 2:	Flags = Val & 0xF;
			break;
		case 3:	PRG = Val & 0xF;
			break;
		case 4:	CHR[0] = Val & 0xF;
			break;
		case 5:	CHR[1] = Val & 0xF;
			break;
		case 6:	CHR[2] = Val & 0xF;
			break;
		case 7:	CHR[3] = Val & 0xF;
			break;
		}
		Sync();
	}
	else
	{
		if (Addr & 4)
			DripSound::Chan[1].Write(Addr & 3, Val);
		else	DripSound::Chan[0].Write(Addr & 3, Val);
	}
}
void	MAPINT	CPUWriteH (int Bank, int Addr, int Val)
{
	if (Addr & 0x400)
		ExtRam1[Addr & 0x3FF] = Val & 3;
	else	ExtRam0[Addr & 0x3FF] = Val & 3;
}

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_UNL_DRIPGAME_J0, (Jumper & 0x80) ? BST_CHECKED : BST_UNCHECKED);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			ConfigCmd = 0x80;
			if (IsDlgButtonChecked(hDlg, IDC_UNL_DRIPGAME_J0) == BST_CHECKED)
				ConfigCmd |= 0x01;
		case IDCANCEL:
			ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

unsigned char	MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (ConfigWindow)
				break;
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_UNL_DRIPGAME), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
		{
			Jumper = (data & 0x01) ? 0x80 : 0x00;
			Sync();
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

BOOL	MAPINT	Load (void)
{
	ConfigWindow = NULL;
	EMU->Mirror_4S();
	ExtRam0 = EMU->GetCHR_Ptr1(0xA);
	ExtRam1 = EMU->GetCHR_Ptr1(0xB);
	UNIF_SetSRAM(8192);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_CPURead4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, CPURead4);
	EMU->SetCPUReadHandler(0x5, CPURead5);
	for (int i = 0x8; i < 0xC; i++)
		EMU->SetCPUWriteHandler(i, CPUWriteL);
	for (int i = 0xC; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, CPUWriteH);

	_PPUReadNT[0] = EMU->GetPPUReadHandler(0x8);
	_PPUReadNT[1] = EMU->GetPPUReadHandler(0x9);
	_PPUReadNT[2] = EMU->GetPPUReadHandler(0xA);
	_PPUReadNT[3] = EMU->GetPPUReadHandler(0xB);
	
	ZeroMemory(&DripSound::Chan, sizeof(DripSound::Chan));
	DripSound::Chan[0].IsEmpty = TRUE;
	DripSound::Chan[1].IsEmpty = TRUE;

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		for (int i = 0; i < 4; i++)
			CHR[i] = i;
		Flags = 0;
		IRQenabled = IRQlatch = 0;
		IRQcounter = 0;
		LastAddr = LastAddrTmp = 0;
		Jumper = 0;
		EMU->SetIRQ(1);
	}

	Sync();
}
void	MAPINT	Unload (void)
{
	if (ConfigWindow)
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}
} // namespace

const MapperInfo MapperInfo_UNL_DRIPGAME
(
	"UNL-DRIPGAME",
	_T("Drip"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	DripSound::MapperSnd,
	Config
);
