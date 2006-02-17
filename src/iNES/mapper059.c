#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"
#include	"resource.h"

static	struct
{
	u8 Jumper;
	HWND ConfigWindow;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetCHR_ROM8(0,Latch.Addr & 0x07);
	EMU->SetPRG_ROM32(0x8,(Latch.Addr & 0x70) >> 4);
	if (Latch.Addr & 0x08)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (Latch.Addr & 0x100)
		for (x = 0x8; x < 0x10; x++)
		{
			EMU->SetPRG_RAM4(x,x-8);
			memset(EMU->GetPRG_Ptr4(x),Mapper.Jumper,0x1000);
		}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	x = Latch_SaveLoad_A(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Jumper)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg,IDC_MAPPER59_J0,(Mapper.Jumper & 0x01) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER59_J1,(Mapper.Jumper & 0x02) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Mapper.Jumper = 0;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER59_J0) == BST_CHECKED)
					Mapper.Jumper |= 0x01;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER59_J1) == BST_CHECKED)
					Mapper.Jumper |= 0x02;
				Sync();
				MessageBox(hDlg,"Please perform a SOFT RESET for this to take effect!","INES.DLL",MB_OK);
			case IDCANCEL:
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
	Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER59),hWnd,(DLGPROC)ConfigProc);
	if (Mapper.ConfigWindow)
		SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	Latch_Destroy();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
	Mapper.ConfigWindow = NULL;
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.ConfigWindow = NULL;

	Latch_Init(Sync,IsHardReset,FALSE);
}

CTMapperInfo	MapperInfo_059 =
{
	59,
	NULL,
	"T3H53",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	Config
};