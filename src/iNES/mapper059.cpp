/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"resource.h"

namespace
{
FCPURead _Read;
uint16_t AddrBits;
uint8_t Jumper;

HWND ConfigWindow;
uint8_t ConfigCmd;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0x0, AddrBits & 0x07);
	if (AddrBits & 0x80)
	{
		EMU->SetPRG_ROM16(0x8, (AddrBits & 0x70) >> 4);
		EMU->SetPRG_ROM16(0xC, (AddrBits & 0x70) >> 4);
	}
	else	EMU->SetPRG_ROM32(0x8, (AddrBits & 0x60) >> 5);
	if (AddrBits & 0x08)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, AddrBits);
	SAVELOAD_BYTE(mode, offset, data, Jumper);

	if (IsLoad(mode))
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
			Jumper = data & 0x3;
		ConfigCmd = 0;
		break;
	}
	return 0;
}

int	MAPINT	Read (int Bank, int Addr)
{
	if (AddrBits & 0x100)
		return (*EMU->OpenBus & 0xFC) | (Jumper & 0x3);
	return _Read(Bank,Addr);
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (AddrBits & 0x200)
		return;
	AddrBits = Addr;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	ConfigWindow = NULL;
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Jumper = 0;
	AddrBits = 0;
	_Read = EMU->GetCPUReadHandler(0x8);
	for (int i = 0x8; i < 0x10; i++)
	{
		EMU->SetCPUReadHandler(i, Read);
		EMU->SetCPUWriteHandler(i, Write);
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

const MapperInfo MapperInfo_059
(
	59,
	_T("T3H53/D1038"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	Config
);
