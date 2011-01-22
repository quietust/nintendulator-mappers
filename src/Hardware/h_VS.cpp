/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_VS.h"
#include	"..\VS\resource.h"

namespace VS
{
FCPURead _Read;
uint8 DipSwitch, Coin;
uint32 CoinDelay;

HWND ConfigWindow;
uint8 ConfigCmd;

void	BlockDialog (HWND hDlg, int dlgItem)
{
	if (!hDlg)
		return;
	if (dlgItem != IDC_VS_DIP0)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP0), FALSE);
	if (dlgItem != IDC_VS_DIP1)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP1), FALSE);
	if (dlgItem != IDC_VS_DIP2)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP2), FALSE);
	if (dlgItem != IDC_VS_DIP3)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP3), FALSE);
	if (dlgItem != IDC_VS_DIP4)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP4), FALSE);
	if (dlgItem != IDC_VS_DIP5)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP5), FALSE);
	if (dlgItem != IDC_VS_DIP6)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP6), FALSE);
	if (dlgItem != IDC_VS_DIP7)	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP7), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN1), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN2), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN3), FALSE);
}

void	UnblockDialog (HWND hDlg)
{
	if (!hDlg)
		return;
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP0), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP1), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP2), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP3), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP4), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP5), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP6), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_VS_DIP7), TRUE);
	if (!CoinDelay)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN1), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN2), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN3), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN1), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN2), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_VS_COIN3), FALSE);
	}
}

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int buttons[8] = {IDC_VS_DIP0,IDC_VS_DIP1,IDC_VS_DIP2,IDC_VS_DIP3,IDC_VS_DIP4,IDC_VS_DIP5,IDC_VS_DIP6,IDC_VS_DIP7};
	switch (message)
	{
	case WM_INITDIALOG:
		if (ConfigCmd)
		{
			if (ConfigCmd < 9)
				BlockDialog(hDlg, ConfigCmd - 1);
			else if (ConfigCmd < 17)
				BlockDialog(hDlg, ConfigCmd - 9);
			else	BlockDialog(hDlg, 0);
		}
		else	UnblockDialog(hDlg);
		for (int i = 0; i < 8; i++)
			CheckDlgButton(hDlg, buttons[i], (DipSwitch & (1 << i)) ? BST_CHECKED : BST_UNCHECKED);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_VS_DIP0:
			if (ConfigCmd == 0)
			{	// if there was no pending command, then disable all other dialog controls
				// so as to avoid command overlaps
				ConfigCmd = 1;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP0) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP0);
			}
			else	// if there WAS a pending command, it had to have been to change the state of this DIP
			{	// so we can abort it and re-enable the others
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP1:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 2;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP1) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP1);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP2:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 3;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP2) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP2);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP3:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 4;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP3) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP3);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP4:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 5;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP4) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP4);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP5:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 6;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP5) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP5);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP6:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 7;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP6) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP6);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_DIP7:
			if (ConfigCmd == 0)
			{
				ConfigCmd = 8;
				if (IsDlgButtonChecked(hDlg, IDC_VS_DIP7) == BST_UNCHECKED)
					ConfigCmd += 8;
				BlockDialog(hDlg, IDC_VS_DIP7);
			}
			else
			{
				UnblockDialog(hDlg);
				ConfigCmd = 0;
			}
			return TRUE;
		case IDC_VS_COIN1:
			ConfigCmd = 17;
			BlockDialog(hDlg, 0);
			return TRUE;
		case IDC_VS_COIN2:
			ConfigCmd = 18;
			BlockDialog(hDlg, 0);
			return TRUE;
		case IDC_VS_COIN3:
			ConfigCmd = 19;
			BlockDialog(hDlg, 0);
			return TRUE;
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_VS), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		switch (data)
		{
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
			// set switch
			DipSwitch |= 1 << (data - 1);
			UnblockDialog(ConfigWindow);
			break;
		case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
			// clear switch
			DipSwitch &= ~(1 << (data - 9));
			UnblockDialog(ConfigWindow);
			break;
		case 17: case 18: case 19:
			if (data == 17)
				Coin |= 0x20;
			if (data == 18)
				Coin |= 0x40;
			if (data == 19)
				Coin |= 0x04;
			CoinDelay = 222222;
			UnblockDialog(ConfigWindow);
			break;
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}


void	Load (void)
{
	ConfigWindow = NULL;
	DipSwitch = 0;	/* clear dipswitches on load */
	Config(CFG_WINDOW, TRUE);	/* open control window automatically */
}

void	Reset (RESET_TYPE ResetType)
{
	CoinDelay = 0;
	Coin = 0;		/* clear coins */

	_Read = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, Read);
	
	ConfigCmd = 0;
	UnblockDialog(ConfigWindow);	/* free up the dialog */
}

void	Unload (void)
{
	if (ConfigWindow)
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, DipSwitch);
	SAVELOAD_BYTE(mode, offset, data, Coin);
	SAVELOAD_LONG(mode, offset, data, CoinDelay);
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((CoinDelay) && (!--CoinDelay))
	{
		Coin = 0;
		UnblockDialog(ConfigWindow);
	}
}

int	MAPINT	Read (int Bank, int Addr)
{
	int Val = _Read(Bank, Addr);
	if (Addr == 0x016)
	{
		Val &= 0x83;
		Val |= Coin;
		Val |= ((DipSwitch & 0x03) << 3);
	}
	else if (Addr == 0x017)
	{
		Val &= 0x03;
		Val |= DipSwitch & 0xFC;
	}
	return Val;
}
} // namespace VS