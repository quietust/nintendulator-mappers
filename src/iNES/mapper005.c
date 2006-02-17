#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC5.h"
#include	"resource.h"

static	struct
{
	HWND ConfigWindow;
}	Mapper;

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			switch (MMC5.WRAMsize)
			{
			case MMC5WRAM_0KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_0);	break;
			case MMC5WRAM_0KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_8);	break;
			case MMC5WRAM_0KB_32KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_32);	break;
			case MMC5WRAM_8KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_0);	break;
			case MMC5WRAM_8KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_8);	break;
			case MMC5WRAM_8KB_32KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_32);	break;
			case MMC5WRAM_32KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_0);	break;
			case MMC5WRAM_32KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_8);	break;
			case MMC5WRAM_32KB_32KB:CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_32);	break;
			}
			SetDlgItemText(hDlg,IDC_MAPPER5_DESCRIBE,"Games and their RAM sizes go here");
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_0) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_0KB_0KB;
					EMU->Set_SRAMSize(0);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_8) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_0KB_8KB;
					EMU->Set_SRAMSize(0);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_32) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_0KB_32KB;
					EMU->Set_SRAMSize(0);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_0) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_8KB_0KB;
					EMU->Set_SRAMSize(8);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_8) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_8KB_8KB;
					EMU->Set_SRAMSize(8);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_32) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_8KB_32KB;
					EMU->Set_SRAMSize(8);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_0) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_32KB_0KB;
					EMU->Set_SRAMSize(32);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_8) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_32KB_8KB;
					EMU->Set_SRAMSize(32);
				}
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_32) == BST_CHECKED)
				{
					MMC5.WRAMsize = MMC5WRAM_32KB_32KB;
					EMU->Set_SRAMSize(32);
				}
				EMU->Load_SRAM();
				MMC5_SyncPRG();	
				MessageBox(hWnd,"Please perform a SOFT reset to ensure proper functionality!","INES.DLL",MB_OK);
			case IDCANCEL:
				Mapper.ConfigWindow = NULL;
				DestroyWindow(hDlg);
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			Mapper.ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;		break;
	}
	return FALSE;
}

static	void	_MAPINT	Config (void)
{
	Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER5),hWnd,(DLGPROC)ConfigProc);
	if (Mapper.ConfigWindow)
		SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC5_Destroy();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
	Mapper.ConfigWindow = NULL;
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	MMC5.WRAMsize = MMC5WRAM_8KB_8KB;

	Mapper.ConfigWindow = NULL;

	MMC5_Init();
}

CTMapperInfo	MapperInfo_005  =
{
	5,
	NULL,
	"MMC5",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	Config
};