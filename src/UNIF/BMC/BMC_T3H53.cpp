/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"
#include	"..\resource.h"

namespace
{
u8 Jumper;
HWND ConfigWindow;
u8 ConfigCmd;

void	Sync (void)
{
	u8 x;
	EMU->SetCHR_ROM8(0, Latch::Addr.s0 & 0x07);
	if (Latch::Addr.b0 & 0x80)
	{
		EMU->SetPRG_ROM16(0x8, (Latch::Addr.s0 & 0x70) >> 4);
		EMU->SetPRG_ROM16(0xC, (Latch::Addr.s0 & 0x70) >> 4);
	}
	else	EMU->SetPRG_ROM32(0x8, (Latch::Addr.s0 & 0x60) >> 5);
	if (Latch::Addr.s0 & 0x08)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (Latch::Addr.s0 & 0x100)
		for (x = 0x8; x < 0x10; x++)
		{
			EMU->SetPRG_RAM4(x, x-8);
			memset(EMU->GetPRG_Ptr4(x), Jumper, 0x1000);
		}
}

LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_BMC_T3H53_J0, (Jumper & 0x01) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_BMC_T3H53_J1, (Jumper & 0x02) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg, IDC_BMC_T3H53_J0) == BST_CHECKED)
					ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg, IDC_BMC_T3H53_J1) == BST_CHECKED)
					ConfigCmd |= 0x02;
			case IDCANCEL:
				DestroyWindow(hDlg);
				ConfigWindow = NULL;
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hDlg);
			ConfigWindow = NULL;
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_BMC_T3H53), hWnd, (DLGPROC)ConfigProc);
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
			Jumper = data & 0x03;
			Sync();
			MessageBox(hWnd, _T("Please perform a SOFT RESET for this to take effect!"), _T("UNIF.DLL"), MB_OK);
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Jumper = 0;

	ConfigCmd = 0;
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
	if (ConfigWindow)
		DestroyWindow(ConfigWindow);
}
} // namespace

const MapperInfo MapperInfo_BMC_T3H53 =
{
	"BMC-T3H53",
	_T("Pirate multicart mapper with dipswitches"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_A,
	NULL,
	Config
};