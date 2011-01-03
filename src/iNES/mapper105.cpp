/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"
#include	"resource.h"

namespace
{
uint32 Counter, MaxCount;
uint8 CounterEnabled;
uint8 InitState;
HWND ConfigWindow;
uint8 ConfigCmd;

void	Sync (void)
{
	uint8 CHRlines = MMC1::GetCHRBankLo();
	MMC1::SyncMirror();
	MMC1::SyncWRAM();
	EMU->SetCHR_RAM8(0, 0);
	switch (InitState)
	{
	case 2:	if (CHRlines & 0x08)
			MMC1::SyncPRG(0x7, 0x8);
		else	EMU->SetPRG_ROM32(0x8, (CHRlines >> 1) & 0x3);
		if (CHRlines & 0x10)
		{
			Counter = 0;
			CounterEnabled = 0;
			EMU->SetIRQ(1);
		}
		else	CounterEnabled = 1;
		return;				break;
	case 0:	if (CHRlines & 0x10)
			InitState++;	break;
	case 1:	if (~CHRlines & 0x10)
			InitState++;	break;
	}
	EMU->SetPRG_ROM32(0x8, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8 Byte = 0;
	offset = MMC1::SaveLoad(mode, offset, data);
	SAVELOAD_LONG(mode, offset, data, Counter);
	if (mode == STATE_SAVE)
		Byte = (uint8)(MaxCount >> 24);
	SAVELOAD_BYTE(mode, offset, data, Byte);
	if (mode == STATE_LOAD)
		MaxCount = Byte << 24;
	SAVELOAD_BYTE(mode, offset, data, CounterEnabled);
	SAVELOAD_BYTE(mode, offset, data, InitState);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (!CounterEnabled)
		return;
	Counter++;
	if (!(Counter % 1789773))
	{
		uint32 SecondsLeft = ((MaxCount | 0x20000000) - Counter) / 1789773;
		EMU->StatusOut(_T("Time left: %02i:%02i"), SecondsLeft / 60, SecondsLeft % 60);
	}
	if (((Counter | (MaxCount ^ 0x1E000000)) & 0x3E000000) == 0x3E000000)
	{
		EMU->StatusOut(_T("Time up!"));
		EMU->SetIRQ(0);
		CounterEnabled = 0;
	}
}

LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_MAPPER105_J0, (MaxCount & 0x02000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_MAPPER105_J1, (MaxCount & 0x04000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_MAPPER105_J2, (MaxCount & 0x08000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_MAPPER105_J3, (MaxCount & 0x10000000) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER105_J0) == BST_CHECKED)
					ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER105_J1) == BST_CHECKED)
					ConfigCmd |= 0x02;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER105_J2) == BST_CHECKED)
					ConfigCmd |= 0x04;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER105_J3) == BST_CHECKED)
					ConfigCmd |= 0x08;
			case IDCANCEL:
				ConfigWindow = NULL;
				DestroyWindow(hDlg);
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;		break;
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER105), hWnd, (DLGPROC)ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
			MaxCount = (data & 0xF) << 25;
		ConfigCmd = 0;
		break;
	}
	return 0;
}

void	MAPINT	Load (void)
{
	MMC1::Load(Sync);
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		MaxCount = 0x04000000;

	Counter = 0;
	CounterEnabled = 0;
	InitState = 0;
	ConfigCmd = 0;

	MMC1::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC1::Unload();
	if (ConfigWindow)
		DestroyWindow(ConfigWindow);
}

uint8 MapperNum = 105;
} // namespace

const MapperInfo MapperInfo_105 =
{
	&MapperNum,
	_T("Nintendo World Championship"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	Config
};