/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"resource.h"

namespace
{
FCPUWrite _Write4;
u8 IRQenabled;
u16_n IRQcounter;
u8 PRG;
u8 Title;
HWND ConfigWindow;
u8 ConfigCmd;

void	Sync (void)
{
	EMU->SetPRG_ROM4(0x5, (8 << 1) | Title);
	EMU->SetPRG_ROM8(0x6, 2);
	EMU->SetPRG_ROM8(0x8, 1);
	EMU->SetPRG_ROM8(0xA, 0);
	EMU->SetPRG_ROM8(0xC, PRG);
	EMU->SetPRG_ROM8(0xE, 9);
	EMU->SetCHR_ROM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, PRG);
	SAVELOAD_BYTE(mode, x, data, Title);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		IRQcounter.s0++;
		if (IRQcounter.s0 >= 4096)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	const u8 PRGbanks[8] = {4,3,4,4,4,7,5,6};
	u16 Loc = (Bank << 12) | Addr;
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if ((Loc & 0x71FF) == 0x4022)
	{
		PRG = PRGbanks[Val & 7];
		Sync();
	}
	if ((Loc & 0x71FF) == 0x0122)
	{
		if (Val & 3)
			IRQenabled = 1;
		else
		{
			IRQcounter.s0 = 0;
			IRQenabled = 0;
			EMU->SetIRQ(1);
		}
	}
}

LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			if (Title == 1)
				CheckRadioButton(hDlg, IDC_MAPPER43_BLANK, IDC_MAPPER43_LOGO, IDC_MAPPER43_BLANK);
			else	CheckRadioButton(hDlg, IDC_MAPPER43_BLANK, IDC_MAPPER43_LOGO, IDC_MAPPER43_LOGO);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER43_BLANK) == BST_CHECKED)
					ConfigCmd |= 0x01;
				else if (IsDlgButtonChecked(hDlg, IDC_MAPPER43_LOGO) == BST_CHECKED)
					ConfigCmd |= 0x00;
				else	MessageBox(hWnd, _T("Impossible - neither radio button checked!"), _T("INES.DLL"), MB_OK);
				MessageBox(hWnd, _T("Please perform a SOFT reset for this to take effect!"), _T("INES.DLL"), MB_OK);
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER43), hWnd, (DLGPROC)ConfigProc);
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
			Title = data & 0x1;
			Sync();
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

void	MAPINT	Load (void)
{
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUReadHandler(0x5, EMU->GetCPUReadHandler(0x8));

	for (x = 0x4; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
	{
		Title = 1;
		PRG = 0;
		IRQenabled = 1;
		IRQcounter.s0 = 0;
	}

	ConfigCmd = 0;
	Sync();
}
void	MAPINT	Unload (void)
{
	if (ConfigWindow)
		DestroyWindow(ConfigWindow);
}

u8 MapperNum = 43;
} // namespace

const MapperInfo MapperInfo_043 =
{
	&MapperNum,
	_T("SMB2j (LF36)"),
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
