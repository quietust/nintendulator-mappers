/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"resource.h"

namespace
{
FCPUWrite _Write4;
uint8_t IRQenabled;
uint16_n IRQcounter;
uint8_t PRG;
uint8_t Title;
HWND ConfigWindow;
uint8_t ConfigCmd;

void	Sync (void)
{
	EMU->SetPRG_ROM4(0x5, (8 << 1) | Title);
	EMU->SetPRG_ROM8(0x6, 2);
	EMU->SetPRG_ROM8(0x8, 1);
	EMU->SetPRG_ROM8(0xA, 0);
	EMU->SetPRG_ROM8(0xC, PRG);
	EMU->SetPRG_ROM8(0xE, 9);
	EMU->SetCHR_ROM8(0x0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Title);

	if (IsLoad(mode))
		Sync();
	return offset;
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
	static const uint8_t PRGbanks[8] = {4,3,4,4,4,7,5,6};
	uint16_t Loc = (Bank << 12) | Addr;
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

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER43), hWnd, ConfigProc);
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

BOOL	MAPINT	Load (void)
{
	ConfigWindow = NULL;
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUReadHandler(0x5, EMU->GetCPUReadHandler(0x8));

	for (int i = 0x4; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

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
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}
} // namespace

const MapperInfo MapperInfo_043
(
	43,
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
);
