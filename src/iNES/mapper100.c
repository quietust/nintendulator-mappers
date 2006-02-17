#include	"..\DLL\d_iNES.h"
#include	"resource.h"

typedef	enum	{ BANK_OPEN, BANK_ROM, BANK_RAM, BANK_NT } BANKTYPE;

static struct
{
	HWND ConfigWindow;
	u32 PRG[5], CHR[16];
	BANKTYPE PRGtype[5], CHRtype[16];
}	Mapper;

static	void	Sync (void)
{
	int x;
	for (x = 0; x < 5; x++)
	{
		if (Mapper.PRGtype[x] == BANK_ROM)
			EMU->SetPRG_ROM8(6 + (x << 1), Mapper.PRG[x]);
		else if (Mapper.PRGtype[x] == BANK_RAM)
			EMU->SetPRG_RAM8(6 + (x << 1), Mapper.PRG[x] & 0xF);
		else
		{
			EMU->SetPRG_OB4(6 + (x << 1));
			EMU->SetPRG_OB4(7 + (x << 1));
		}
	}

	for (x = 0; x < 16; x++)
	{
		if (Mapper.CHRtype[x] == BANK_ROM)
			EMU->SetCHR_ROM1(x, Mapper.CHR[x]);
		else if (Mapper.CHRtype[x] == BANK_RAM)
			EMU->SetCHR_RAM1(x, Mapper.CHR[x]);
		else if (Mapper.CHRtype[x] == BANK_NT)
			EMU->SetCHR_NT1(x, Mapper.CHR[x]);
		else	EMU->SetCHR_OB1(x);
	}
}

static	void	GetPRGBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckOpen, int Slot)
{
	SetDlgItemInt(hDlg, Editbox, Mapper.PRG[Slot], TRUE);
	if (Mapper.PRGtype[Slot] == BANK_ROM)
		CheckDlgButton(hDlg, CheckROM, BST_CHECKED);
	else if (Mapper.PRGtype[Slot] == BANK_RAM)
		CheckDlgButton(hDlg, CheckRAM, BST_CHECKED);
	else if (Mapper.PRGtype[Slot] == BANK_OPEN)
		CheckDlgButton(hDlg, CheckOpen, BST_CHECKED);
	else	EMU->DbgOut(_T("Impossible: no type selected for PRG bank %i!"), Slot);
}

static	void	SetPRGBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckOpen, int Slot)
{
	int Bank = GetDlgItemInt(hDlg, Editbox, NULL, TRUE);
	if (IsDlgButtonChecked(hDlg, CheckROM) == BST_CHECKED)
	{
		Mapper.PRGtype[Slot] = BANK_ROM;
		Mapper.PRG[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckRAM) == BST_CHECKED)
	{
		Mapper.PRGtype[Slot] = BANK_RAM;
		Mapper.PRG[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckOpen) == BST_CHECKED)
	{
		Mapper.PRGtype[Slot] = BANK_OPEN;
		Mapper.PRG[Slot] = 0;
	}
	else	EMU->DbgOut(_T("Impossible: no type selected for PRG bank %i!"), Slot);
	Sync();
}

static	void	GetCHRBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckNT, int CheckOpen, int Slot)
{
	SetDlgItemInt(hDlg, Editbox, Mapper.CHR[Slot], TRUE);
	if (Mapper.CHRtype[Slot] == BANK_ROM)
		CheckDlgButton(hDlg, CheckROM, BST_CHECKED);
	else if (Mapper.CHRtype[Slot] == BANK_RAM)
		CheckDlgButton(hDlg, CheckRAM, BST_CHECKED);
	else if (Mapper.CHRtype[Slot] == BANK_NT)
		CheckDlgButton(hDlg, CheckNT, BST_CHECKED);
	else if (Mapper.CHRtype[Slot] == BANK_OPEN)
		CheckDlgButton(hDlg, CheckOpen, BST_CHECKED);
	else	EMU->DbgOut(_T("Impossible: no type selected for CHR bank %i!"), Slot);
	if ((Slot >= 8) && (Slot < 12))
		GetCHRBank(hDlg, Editbox, CheckROM, CheckRAM, CheckNT, CheckOpen, Slot + 4);
}

static	void	SetCHRBank(HWND hDlg, int Editbox, int CheckROM, int CheckRAM, int CheckNT, int CheckOpen, int Slot)
{
	int Bank = GetDlgItemInt(hDlg, Editbox, NULL, TRUE);
	if (IsDlgButtonChecked(hDlg, CheckROM) == BST_CHECKED)
	{
		Mapper.CHRtype[Slot] = BANK_ROM;
		Mapper.CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckRAM) == BST_CHECKED)
	{
		Mapper.CHRtype[Slot] = BANK_RAM;
		Mapper.CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckNT) == BST_CHECKED)
	{
		Mapper.CHRtype[Slot] = BANK_NT;
		Mapper.CHR[Slot] = Bank;
	}
	else if (IsDlgButtonChecked(hDlg, CheckOpen) == BST_CHECKED)
	{
		Mapper.CHRtype[Slot] = BANK_OPEN;
		Mapper.CHR[Slot] = 0;
	}
	else	EMU->DbgOut(_T("Impossible: no type selected for CHR bank %i!"), Slot);
	Sync();
	if ((Slot >= 8) && (Slot < 12))
		SetCHRBank(hDlg, Editbox, CheckROM, CheckRAM, CheckNT, CheckOpen, Slot + 4);
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
				Mapper.ConfigWindow = NULL;
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hDlg);
			Mapper.ConfigWindow = NULL;
			return TRUE;		break;
	}
	return FALSE;
}

static	unsigned char	_MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER100),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
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

static	void	_MAPINT	Load (void)
{
	int x;
	for (x = 0; x < 5; x++)
	{
		Mapper.PRG[x] = 0;
		Mapper.PRGtype[x] = BANK_OPEN;
	}
	for (x = 0; x < 16; x++)
	{
		Mapper.CHR[x] = 0;
		Mapper.CHRtype[x] = BANK_OPEN;
	}
	Mapper.ConfigWindow = NULL;
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	Sync();
}
static	void	_MAPINT	Unload (void)
{
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
}

static	u8 MapperNum = 100;
CTMapperInfo	MapperInfo_100 =
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
