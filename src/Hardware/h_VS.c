#include	"h_VS.h"
#include	"..\VS\resource.h"

static	TVS	VS;

static	void	BlockDialog (HWND hDlg, int dlgItem)
{
	if (!hDlg)
		return;
	if (dlgItem != IDC_VS_DIP0)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP0),FALSE);
	if (dlgItem != IDC_VS_DIP1)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP1),FALSE);
	if (dlgItem != IDC_VS_DIP2)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP2),FALSE);
	if (dlgItem != IDC_VS_DIP3)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP3),FALSE);
	if (dlgItem != IDC_VS_DIP4)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP4),FALSE);
	if (dlgItem != IDC_VS_DIP5)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP5),FALSE);
	if (dlgItem != IDC_VS_DIP6)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP6),FALSE);
	if (dlgItem != IDC_VS_DIP7)	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP7),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN1),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN2),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN3),FALSE);
}

static	void	UnblockDialog (HWND hDlg)
{
	if (!hDlg)
		return;
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP0),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP1),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP2),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP3),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP4),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP5),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP6),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_VS_DIP7),TRUE);
	if (!VS.CoinDelay)
	{
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN1),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN2),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN3),TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN1),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN2),FALSE);
		EnableWindow(GetDlgItem(hDlg,IDC_VS_COIN3),FALSE);
	}
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const int buttons[8] = {IDC_VS_DIP0,IDC_VS_DIP1,IDC_VS_DIP2,IDC_VS_DIP3,IDC_VS_DIP4,IDC_VS_DIP5,IDC_VS_DIP6,IDC_VS_DIP7};
	int i;
	switch (message)
	{
	case WM_INITDIALOG:
		if (VS.ConfigCmd)
		{
			if (VS.ConfigCmd < 9)
				BlockDialog(hDlg,VS.ConfigCmd - 1);
			else if (VS.ConfigCmd < 17)
				BlockDialog(hDlg,VS.ConfigCmd - 9);
			else	BlockDialog(hDlg,0);
		}
		else	UnblockDialog(hDlg);
		for (i = 0; i < 8; i++)
			CheckDlgButton(hDlg,buttons[i],(VS.DipSwitch & (1 << i)) ? BST_CHECKED : BST_UNCHECKED);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_VS_DIP0:
			if (VS.ConfigCmd == 0)
			{	// if there was no pending command, then disable all other dialog controls
				// so as to avoid command overlaps
				VS.ConfigCmd = 1;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP0) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP0);
			}
			else	// if there WAS a pending command, it was to change the state of this DIP
			{	// so we can abort it and re-enable the others
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP1:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 2;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP1) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP1);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP2:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 3;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP2) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP2);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP3:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 4;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP3) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP3);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP4:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 5;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP4) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP4);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP5:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 6;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP5) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP5);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP6:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 7;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP6) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP6);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_DIP7:
			if (VS.ConfigCmd == 0)
			{
				VS.ConfigCmd = 8;
				if (IsDlgButtonChecked(hDlg,IDC_VS_DIP7) == BST_UNCHECKED)
					VS.ConfigCmd += 8;
				BlockDialog(hDlg,IDC_VS_DIP7);
			}
			else
			{
				UnblockDialog(hDlg);
				VS.ConfigCmd = 0;
			}
			return TRUE;		break;
		case IDC_VS_COIN1:
			VS.ConfigCmd = 17;
			BlockDialog(hDlg, 0);
			return TRUE;		break;
		case IDC_VS_COIN2:
			VS.ConfigCmd = 18;
			BlockDialog(hDlg, 0);
			return TRUE;		break;
		case IDC_VS_COIN3:
			VS.ConfigCmd = 19;
			BlockDialog(hDlg, 0);
			return TRUE;		break;
		case IDCLOSE:
			VS.ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;		break;
		}
		break;
	case WM_CLOSE:
		VS.ConfigWindow = NULL;
		DestroyWindow(hDlg);
		return TRUE;		break;
	}
	return FALSE;
}

unsigned char	MAPINT	VS_Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (VS.ConfigWindow)
				break;
			VS.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_VS),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(VS.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return VS.ConfigCmd;
		break;
	case CFG_CMD:
		switch (data)
		{
		case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
			// set switch
			VS.DipSwitch |= 1 << (data - 1);
			UnblockDialog(VS.ConfigWindow);
			break;
		case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
			// clear switch
			VS.DipSwitch &= ~(1 << (data - 9));
			UnblockDialog(VS.ConfigWindow);
			break;
		case 17: case 18: case 19:
			if (data == 17)
				VS.Coin |= 0x20;
			if (data == 18)
				VS.Coin |= 0x40;
			if (data == 19)
				VS.Coin |= 0x04;
			VS.CoinDelay = 888888;
			UnblockDialog(VS.ConfigWindow);
			break;
		}
		VS.ConfigCmd = 0;
		break;
	}
	return 0;
}


void	VS_Load (void)
{
	VS.ConfigWindow = NULL;
	VS.DipSwitch = 0;	/* clear dipswitches on load */
	VS_Config(CFG_WINDOW,TRUE);	/* open control window automatically */
}

void	VS_Reset (RESET_TYPE ResetType)
{
	VS.CoinDelay = 0;
	VS.Coin = 0;		/* clear coins */

	VS.Read = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,VS_Read);
	
	VS.ConfigCmd = 0;
	UnblockDialog(VS.ConfigWindow);	/* free up the dialog */
}

void	VS_Unload (void)
{
	if (VS.ConfigWindow)
		DestroyWindow(VS.ConfigWindow);
}

int	MAPINT	VS_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,VS.DipSwitch)
	SAVELOAD_BYTE(mode,x,data,VS.Coin)
	SAVELOAD_LONG(mode,x,data,VS.CoinDelay)
	return x;
}

void	MAPINT	VS_CPUCycle (void)
{
	if ((VS.CoinDelay) && (!--VS.CoinDelay))
	{
		VS.Coin = 0;
		UnblockDialog(VS.ConfigWindow);
	}
}

int	MAPINT	VS_Read (int Bank, int Addr)
{
	int Val = VS.Read(Bank,Addr);
	if (Addr == 0x016)
	{
		Val &= 0x83;
		Val |= VS.Coin;
		Val |= ((VS.DipSwitch & 0x03) << 3);
	}
	else if (Addr == 0x017)
	{
		Val &= 0x03;
		Val |= VS.DipSwitch & 0xFC;
	}
	return Val;
}