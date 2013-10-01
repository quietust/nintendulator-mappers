/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"
#include	"resource.h"

namespace
{
uint8 Jumper;
HWND ConfigWindow;
uint8 ConfigCmd;
uint8 JumperData[0x1000];

void	Sync (void)
{
	EMU->SetCHR_ROM8(0, Latch::Addr.s0 & 0x07);
	EMU->SetPRG_ROM32(0x8, (Latch::Addr.s0 & 0x70) >> 4);
	if (Latch::Addr.s0 & 0x08)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (Latch::Addr.s0 & 0x100)
	{
		memset(JumperData, Jumper, 0x1000);
		for (int i = 0x8; i < 0x10; i++)
			EMU->SetPRG_Ptr4(i, JumperData, FALSE);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = Latch::SaveLoad_A(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, Jumper);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_MAPPER59_J0, (Jumper & 0x01) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_MAPPER59_J1, (Jumper & 0x02) ? BST_CHECKED : BST_UNCHECKED);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			ConfigCmd = 0x80;
			if (IsDlgButtonChecked(hDlg, IDC_MAPPER59_J0) == BST_CHECKED)
				ConfigCmd |= 0x01;
			if (IsDlgButtonChecked(hDlg, IDC_MAPPER59_J1) == BST_CHECKED)
				ConfigCmd |= 0x02;
			MessageBox(hDlg, _T("Please perform a SOFT RESET for this to take effect!"), _T("INES.DLL"), MB_OK);
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER59), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
		{
			Jumper = data & 0x3;
			Sync();
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
	ConfigWindow = NULL;
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
	ConfigCmd = 0;
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
	if (ConfigWindow)
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}

uint8 MapperNum = 59;
} // namespace

const MapperInfo MapperInfo_059 =
{
	&MapperNum,
	_T("T3H53"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	Config
};