#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"
#include	"resource.h"

static	struct
{
	u8 Jumper;
	HWND ConfigWindow;
	u8 ConfigCmd;
}	Mapper;

static	u8 JumperData[0x1000];

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Latch.Addr.s0 & 0x07);
	EMU->SetPRG_ROM32(0x8,(Latch.Addr.s0 & 0x70) >> 4);
	if (Latch.Addr.s0 & 0x08)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (Latch.Addr.s0 & 0x100)
	{
		u8 x;
		memset(JumperData,Mapper.Jumper,0x1000);
		for (x = 0x8; x < 0x10; x++)
			EMU->SetPRG_Ptr4(x,JumperData,FALSE);
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch_SaveLoad_A(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Jumper);
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
				Mapper.ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER59_J0) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER59_J1) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x02;
				MessageBox(hDlg,_T("Please perform a SOFT RESET for this to take effect!"),_T("INES.DLL"),MB_OK);
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

static	unsigned char	MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER59),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		return Mapper.ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
		{
			Mapper.Jumper = data & 0x3;
			Sync();
		}
		Mapper.ConfigCmd = 0;
		break;
	}
	return 0;
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
	Mapper.ConfigWindow = NULL;
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
	Mapper.ConfigCmd = 0;
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
}

static	u8 MapperNum = 59;
CTMapperInfo	MapperInfo_059 =
{
	&MapperNum,
	_T("T3H53"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	Config
};