/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"resource.h"

namespace
{
enum BANKTYPE	{ BANK_OPEN, BANK_ROM, BANK_RAM, BANK_NT };

HWND ConfigWindow;
uint32 PRG[5], CHR[16];
BANKTYPE PRGtype[5], CHRtype[16];

void	Sync (void)
{
	for (int i = 0; i < 5; i++)
	{
		if (PRGtype[i] == BANK_ROM)
			EMU->SetPRG_ROM8(6 + (i << 1), PRG[i]);
		else if (PRGtype[i] == BANK_RAM)
			EMU->SetPRG_RAM8(6 + (i << 1), PRG[i] & 0xF);
		else
		{
			EMU->SetPRG_OB4(6 + (i << 1));
			EMU->SetPRG_OB4(7 + (i << 1));
		}
	}

	for (int i = 0; i < 16; i++)
	{
		if (CHRtype[i] == BANK_ROM)
			EMU->SetCHR_ROM1(i, CHR[i]);
		else if (CHRtype[i] == BANK_RAM)
			EMU->SetCHR_RAM1(i, CHR[i]);
		else if (CHRtype[i] == BANK_NT)
			EMU->SetCHR_NT1(i, CHR[i]);
		else	EMU->SetCHR_OB1(i);
	}
}

void	GetPRGBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckOpen, int Slot)
{
	SetDlgItemInt(hDlg, Editbox, PRG[Slot], TRUE);
	if (PRGtype[Slot] == BANK_ROM)
		CheckDlgButton(hDlg, CheckROM, BST_CHECKED);
	else if (PRGtype[Slot] == BANK_RAM)
		CheckDlgButton(hDlg, CheckRAM, BST_CHECKED);
	else if (PRGtype[Slot] == BANK_OPEN)
		CheckDlgButton(hDlg, CheckOpen, BST_CHECKED);
	else	EMU->DbgOut(_T("Impossible: no type selected for PRG bank %i!"), Slot);
}

void	SetPRGBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckOpen, int Slot)
{
	int Bank = GetDlgItemInt(hDlg, Editbox, NULL, TRUE);
	if (IsDlgButtonChecked(hDlg, CheckROM) == BST_CHECKED)
	{
		PRGtype[Slot] = BANK_ROM;
		PRG[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckRAM) == BST_CHECKED)
	{
		PRGtype[Slot] = BANK_RAM;
		PRG[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckOpen) == BST_CHECKED)
	{
		PRGtype[Slot] = BANK_OPEN;
		PRG[Slot] = 0;
	}
	else	EMU->DbgOut(_T("Impossible: no type selected for PRG bank %i!"), Slot);
	Sync();
}

void	GetCHRBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckNT, int CheckOpen, int Slot)
{
	SetDlgItemInt(hDlg, Editbox, CHR[Slot], TRUE);
	if (CHRtype[Slot] == BANK_ROM)
		CheckDlgButton(hDlg, CheckROM, BST_CHECKED);
	else if (CHRtype[Slot] == BANK_RAM)
		CheckDlgButton(hDlg, CheckRAM, BST_CHECKED);
	else if (CHRtype[Slot] == BANK_NT)
		CheckDlgButton(hDlg, CheckNT, BST_CHECKED);
	else if (CHRtype[Slot] == BANK_OPEN)
		CheckDlgButton(hDlg, CheckOpen, BST_CHECKED);
	else	EMU->DbgOut(_T("Impossible: no type selected for CHR bank %i!"), Slot);
	if ((Slot >= 8) && (Slot < 12))
		GetCHRBank(hDlg, Editbox, CheckROM, CheckRAM, CheckNT, CheckOpen, Slot + 4);
}

void	SetCHRBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckNT, int CheckOpen, int Slot)
{
	int Bank = GetDlgItemInt(hDlg, Editbox, NULL, TRUE);
	if (IsDlgButtonChecked(hDlg, CheckROM) == BST_CHECKED)
	{
		CHRtype[Slot] = BANK_ROM;
		CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckRAM) == BST_CHECKED)
	{
		CHRtype[Slot] = BANK_RAM;
		CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckNT) == BST_CHECKED)
	{
		CHRtype[Slot] = BANK_NT;
		CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckOpen) == BST_CHECKED)
	{
		CHRtype[Slot] = BANK_OPEN;
		CHR[Slot] = 0;
	}
	else	EMU->DbgOut(_T("Impossible: no type selected for CHR bank %i!"), Slot);
	Sync();
	if ((Slot >= 8) && (Slot < 12))
		SetCHRBank(hDlg, Editbox, CheckROM, CheckRAM, CheckNT, CheckOpen, Slot + 4);
}

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			GetPRGBank(hDlg, IDC_MAPPER100_PRG67, IDC_MAPPER100_PRG67_ROM, IDC_MAPPER100_PRG67_RAM, IDC_MAPPER100_PRG67_OPEN, 0);
			GetPRGBank(hDlg, IDC_MAPPER100_PRG89, IDC_MAPPER100_PRG89_ROM, IDC_MAPPER100_PRG89_RAM, IDC_MAPPER100_PRG89_OPEN, 1);
			GetPRGBank(hDlg, IDC_MAPPER100_PRGAB, IDC_MAPPER100_PRGAB_ROM, IDC_MAPPER100_PRGAB_RAM, IDC_MAPPER100_PRGAB_OPEN, 2);
			GetPRGBank(hDlg, IDC_MAPPER100_PRGCD, IDC_MAPPER100_PRGCD_ROM, IDC_MAPPER100_PRGCD_RAM, IDC_MAPPER100_PRGCD_OPEN, 3);
			GetPRGBank(hDlg, IDC_MAPPER100_PRGEF, IDC_MAPPER100_PRGEF_ROM, IDC_MAPPER100_PRGEF_RAM, IDC_MAPPER100_PRGEF_OPEN, 4);

			GetCHRBank(hDlg, IDC_MAPPER100_CHR0, IDC_MAPPER100_CHR0_ROM, IDC_MAPPER100_CHR0_RAM, IDC_MAPPER100_CHR0_NT, IDC_MAPPER100_CHR0_OPEN, 0);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR1, IDC_MAPPER100_CHR1_ROM, IDC_MAPPER100_CHR1_RAM, IDC_MAPPER100_CHR1_NT, IDC_MAPPER100_CHR1_OPEN, 1);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR2, IDC_MAPPER100_CHR2_ROM, IDC_MAPPER100_CHR2_RAM, IDC_MAPPER100_CHR2_NT, IDC_MAPPER100_CHR2_OPEN, 2);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR3, IDC_MAPPER100_CHR3_ROM, IDC_MAPPER100_CHR3_RAM, IDC_MAPPER100_CHR3_NT, IDC_MAPPER100_CHR3_OPEN, 3);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR4, IDC_MAPPER100_CHR4_ROM, IDC_MAPPER100_CHR4_RAM, IDC_MAPPER100_CHR4_NT, IDC_MAPPER100_CHR4_OPEN, 4);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR5, IDC_MAPPER100_CHR5_ROM, IDC_MAPPER100_CHR5_RAM, IDC_MAPPER100_CHR5_NT, IDC_MAPPER100_CHR5_OPEN, 5);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR6, IDC_MAPPER100_CHR6_ROM, IDC_MAPPER100_CHR6_RAM, IDC_MAPPER100_CHR6_NT, IDC_MAPPER100_CHR6_OPEN, 6);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR7, IDC_MAPPER100_CHR7_ROM, IDC_MAPPER100_CHR7_RAM, IDC_MAPPER100_CHR7_NT, IDC_MAPPER100_CHR7_OPEN, 7);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR8, IDC_MAPPER100_CHR8_ROM, IDC_MAPPER100_CHR8_RAM, IDC_MAPPER100_CHR8_NT, IDC_MAPPER100_CHR8_OPEN, 8);
			GetCHRBank(hDlg, IDC_MAPPER100_CHR9, IDC_MAPPER100_CHR9_ROM, IDC_MAPPER100_CHR9_RAM, IDC_MAPPER100_CHR9_NT, IDC_MAPPER100_CHR9_OPEN, 9);
			GetCHRBank(hDlg, IDC_MAPPER100_CHRA, IDC_MAPPER100_CHRA_ROM, IDC_MAPPER100_CHRA_RAM, IDC_MAPPER100_CHRA_NT, IDC_MAPPER100_CHRA_OPEN, 10);
			GetCHRBank(hDlg, IDC_MAPPER100_CHRB, IDC_MAPPER100_CHRB_ROM, IDC_MAPPER100_CHRB_RAM, IDC_MAPPER100_CHRB_NT, IDC_MAPPER100_CHRB_OPEN, 11);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDAPPLY:
				SetPRGBank(hDlg, IDC_MAPPER100_PRG67, IDC_MAPPER100_PRG67_ROM, IDC_MAPPER100_PRG67_RAM, IDC_MAPPER100_PRG67_OPEN, 0);
				SetPRGBank(hDlg, IDC_MAPPER100_PRG89, IDC_MAPPER100_PRG89_ROM, IDC_MAPPER100_PRG89_RAM, IDC_MAPPER100_PRG89_OPEN, 1);
				SetPRGBank(hDlg, IDC_MAPPER100_PRGAB, IDC_MAPPER100_PRGAB_ROM, IDC_MAPPER100_PRGAB_RAM, IDC_MAPPER100_PRGAB_OPEN, 2);
				SetPRGBank(hDlg, IDC_MAPPER100_PRGCD, IDC_MAPPER100_PRGCD_ROM, IDC_MAPPER100_PRGCD_RAM, IDC_MAPPER100_PRGCD_OPEN, 3);
				SetPRGBank(hDlg, IDC_MAPPER100_PRGEF, IDC_MAPPER100_PRGEF_ROM, IDC_MAPPER100_PRGEF_RAM, IDC_MAPPER100_PRGEF_OPEN, 4);

				SetCHRBank(hDlg, IDC_MAPPER100_CHR0, IDC_MAPPER100_CHR0_ROM, IDC_MAPPER100_CHR0_RAM, IDC_MAPPER100_CHR0_NT, IDC_MAPPER100_CHR0_OPEN, 0);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR1, IDC_MAPPER100_CHR1_ROM, IDC_MAPPER100_CHR1_RAM, IDC_MAPPER100_CHR1_NT, IDC_MAPPER100_CHR1_OPEN, 1);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR2, IDC_MAPPER100_CHR2_ROM, IDC_MAPPER100_CHR2_RAM, IDC_MAPPER100_CHR2_NT, IDC_MAPPER100_CHR2_OPEN, 2);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR3, IDC_MAPPER100_CHR3_ROM, IDC_MAPPER100_CHR3_RAM, IDC_MAPPER100_CHR3_NT, IDC_MAPPER100_CHR3_OPEN, 3);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR4, IDC_MAPPER100_CHR4_ROM, IDC_MAPPER100_CHR4_RAM, IDC_MAPPER100_CHR4_NT, IDC_MAPPER100_CHR4_OPEN, 4);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR5, IDC_MAPPER100_CHR5_ROM, IDC_MAPPER100_CHR5_RAM, IDC_MAPPER100_CHR5_NT, IDC_MAPPER100_CHR5_OPEN, 5);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR6, IDC_MAPPER100_CHR6_ROM, IDC_MAPPER100_CHR6_RAM, IDC_MAPPER100_CHR6_NT, IDC_MAPPER100_CHR6_OPEN, 6);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR7, IDC_MAPPER100_CHR7_ROM, IDC_MAPPER100_CHR7_RAM, IDC_MAPPER100_CHR7_NT, IDC_MAPPER100_CHR7_OPEN, 7);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR8, IDC_MAPPER100_CHR8_ROM, IDC_MAPPER100_CHR8_RAM, IDC_MAPPER100_CHR8_NT, IDC_MAPPER100_CHR8_OPEN, 8);
				SetCHRBank(hDlg, IDC_MAPPER100_CHR9, IDC_MAPPER100_CHR9_ROM, IDC_MAPPER100_CHR9_RAM, IDC_MAPPER100_CHR9_NT, IDC_MAPPER100_CHR9_OPEN, 9);
				SetCHRBank(hDlg, IDC_MAPPER100_CHRA, IDC_MAPPER100_CHRA_ROM, IDC_MAPPER100_CHRA_RAM, IDC_MAPPER100_CHRA_NT, IDC_MAPPER100_CHRA_OPEN, 10);
				SetCHRBank(hDlg, IDC_MAPPER100_CHRB, IDC_MAPPER100_CHRB_ROM, IDC_MAPPER100_CHRB_RAM, IDC_MAPPER100_CHRB_NT, IDC_MAPPER100_CHRB_OPEN, 11);
				return TRUE;		break;
			case IDCLOSE:
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER100), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		break;
	case CFG_CMD:
		break;
	}
	return 0;
}

void	MAPINT	Load (void)
{
	for (int i = 0; i < 5; i++)
	{
		PRG[i] = 0;
		PRGtype[i] = BANK_OPEN;
	}
	for (int i = 0; i < 16; i++)
	{
		CHR[i] = 0;
		CHRtype[i] = BANK_OPEN;
	}
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Sync();
}
void	MAPINT	Unload (void)
{
	if (ConfigWindow)
		DestroyWindow(ConfigWindow);
}

uint8 MapperNum = 100;
} // namespace

const MapperInfo MapperInfo_100 =
{
	&MapperNum,
	_T("Debugging Mapper"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	NULL,
	NULL,
	Config
};
