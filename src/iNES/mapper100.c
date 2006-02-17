#include	"..\DLL\d_iNES.h"
#include	"resource.h"

static struct
{
	HWND ConfigWindow;
}	Mapper;

void	GetPRGBank(HWND hDlg, int Editbox, int Checkbox, int Loc)
{
	int Bank;
	Bank = EMU->GetPRG_ROM4(Loc);
	CheckDlgButton(hDlg,Checkbox,BST_UNCHECKED);
	if (Bank == -1)
	{
		Bank = EMU->GetPRG_RAM4(Loc);

		if (Bank == -1)
			SetDlgItemInt(hDlg,Editbox,-1,TRUE);
		else
		{
			SetDlgItemInt(hDlg,Editbox,Bank >> 1,TRUE);
			CheckDlgButton(hDlg,Checkbox,BST_CHECKED);
		}
	}
	else	SetDlgItemInt(hDlg,Editbox,Bank >> 1,TRUE);
}

void	GetCHRBank(HWND hDlg, int Editbox, int Checkbox, int Loc)
{
	int Bank;
	Bank = EMU->GetCHR_ROM1(Loc);
	CheckDlgButton(hDlg,Checkbox,BST_UNCHECKED);
	if (Bank == -1)
	{
		Bank = EMU->GetCHR_RAM1(Loc);
		if (Bank != -1)
			CheckDlgButton(hDlg,Checkbox,BST_CHECKED);
	}
	SetDlgItemInt(hDlg,Editbox,Bank,TRUE);
}

void	SetPRGBank(HWND hDlg, int Editbox, int Checkbox, int Loc)
{
	int Bank = GetDlgItemInt(hDlg,Editbox,NULL,TRUE);
	if (Bank == -1)
	{
		EMU->SetPRG_OB4(Loc);
		EMU->SetPRG_OB4(Loc+1);
	}
	else if (IsDlgButtonChecked(hDlg,Checkbox) == BST_CHECKED)
		EMU->SetPRG_RAM8(Loc,Bank);
	else	EMU->SetPRG_ROM8(Loc,Bank);
}

void	SetCHRBank(HWND hDlg, int Editbox, int Checkbox, int Loc)
{
	int Bank = GetDlgItemInt(hDlg,Editbox,NULL,TRUE);
	if (Loc == 0)
		EMU->SetCHR_ROM8(Loc,Bank);
	return;
	if (Bank == -1)
		EMU->SetCHR_OB1(Loc);
	else if (IsDlgButtonChecked(hDlg,Checkbox) == BST_CHECKED)
		EMU->SetCHR_RAM1(Loc,Bank);
	else	EMU->SetCHR_ROM1(Loc,Bank);
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			GetPRGBank(hDlg,IDC_MAPPER100_PRG67,IDC_MAPPER100_PRG67R,0x6);
			GetPRGBank(hDlg,IDC_MAPPER100_PRG89,IDC_MAPPER100_PRG89R,0x8);
			GetPRGBank(hDlg,IDC_MAPPER100_PRGAB,IDC_MAPPER100_PRGABR,0xA);
			GetPRGBank(hDlg,IDC_MAPPER100_PRGCD,IDC_MAPPER100_PRGCDR,0xC);
			GetPRGBank(hDlg,IDC_MAPPER100_PRGEF,IDC_MAPPER100_PRGEFR,0xE);

			GetCHRBank(hDlg,IDC_MAPPER100_CHR0,IDC_MAPPER100_CHR0R,0);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR1,IDC_MAPPER100_CHR1R,1);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR2,IDC_MAPPER100_CHR2R,2);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR3,IDC_MAPPER100_CHR3R,3);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR4,IDC_MAPPER100_CHR4R,4);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR5,IDC_MAPPER100_CHR5R,5);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR6,IDC_MAPPER100_CHR6R,6);
			GetCHRBank(hDlg,IDC_MAPPER100_CHR7,IDC_MAPPER100_CHR7R,7);
			
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDAPPLY:
				SetPRGBank(hDlg,IDC_MAPPER100_PRG67,IDC_MAPPER100_PRG67R,0x6);
				SetPRGBank(hDlg,IDC_MAPPER100_PRG89,IDC_MAPPER100_PRG89R,0x8);
				SetPRGBank(hDlg,IDC_MAPPER100_PRGAB,IDC_MAPPER100_PRGABR,0xA);
				SetPRGBank(hDlg,IDC_MAPPER100_PRGCD,IDC_MAPPER100_PRGCDR,0xC);
				SetPRGBank(hDlg,IDC_MAPPER100_PRGEF,IDC_MAPPER100_PRGEFR,0xE);

				SetCHRBank(hDlg,IDC_MAPPER100_CHR0,IDC_MAPPER100_CHR0R,0);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR1,IDC_MAPPER100_CHR1R,1);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR2,IDC_MAPPER100_CHR2R,2);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR3,IDC_MAPPER100_CHR3R,3);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR4,IDC_MAPPER100_CHR4R,4);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR5,IDC_MAPPER100_CHR5R,5);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR6,IDC_MAPPER100_CHR6R,6);
				SetCHRBank(hDlg,IDC_MAPPER100_CHR7,IDC_MAPPER100_CHR7R,7);
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

static	void	_MAPINT	Config (void)
{
	Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER100),hWnd,(DLGPROC)ConfigProc);
	if (Mapper.ConfigWindow)
		SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
	Mapper.ConfigWindow = NULL;
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetPRG_ROM32(0x8,-1);
	EMU->SetCHR_RAM8(0,0);

	Mapper.ConfigWindow = NULL;
}

CTMapperInfo	MapperInfo_100 =
{
	100,
	NULL,
	"Debugging Mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	NULL,
	NULL,
	Config
};
