#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"
#include	"resource.h"
#include	<stdio.h>

static	struct
{
	u32 Counter, MaxCount;
	u8 CounterEnabled;
	u8 InitState;
	HWND ConfigWindow;
}	Mapper;

static	void	Sync (void)
{
	u8 CHRlines = MMC1_GetCHRBankLo();
	MMC1_SyncMirror();
	MMC1_SyncWRAM();
	EMU->SetCHR_RAM8(0,0);
	switch (Mapper.InitState)
	{
	case 2:	if (CHRlines & 0x08)
			MMC1_SyncPRG(0x7,0x8);
		else	EMU->SetPRG_ROM32(0x8,(CHRlines >> 1) & 0x3);
		if (CHRlines & 0x10)
		{
			Mapper.Counter = 0;
			Mapper.CounterEnabled = 0;
			EMU->SetIRQ(1);
		}
		else	Mapper.CounterEnabled = 1;
		return;				break;
	case 0:	if (CHRlines & 0x10)
			Mapper.InitState++;	break;
	case 1:	if (~CHRlines & 0x10)
			Mapper.InitState++;	break;
	}
	EMU->SetPRG_ROM32(0x8,0);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 Byte;
	x = MMC1_SaveLoad(mode,x,data);
	SAVELOAD_LONG(mode,x,data,Mapper.Counter)
	if (mode == STATE_SAVE)
		Byte = (u8)(Mapper.MaxCount >> 24);
	SAVELOAD_BYTE(mode,x,data,Byte)
	if (mode == STATE_LOAD)
		Mapper.MaxCount = Byte << 24;
	SAVELOAD_BYTE(mode,x,data,Mapper.CounterEnabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.InitState)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (!Mapper.CounterEnabled)
		return;
	Mapper.Counter++;
	if (!(Mapper.Counter % 1789773))
	{
		char tmp[32];
		u32 SecondsLeft = ((Mapper.MaxCount | 0x20000000) - Mapper.Counter) / 1789773;
		sprintf(tmp,"Time left: %02i:%02i",SecondsLeft / 60,SecondsLeft % 60);
		EMU->StatusOut(tmp);
	}
	if (((Mapper.Counter | (Mapper.MaxCount ^ 0x1E000000)) & 0x3E000000) == 0x3E000000)
	{
		EMU->StatusOut("Time up!");
		EMU->SetIRQ(0);
		Mapper.CounterEnabled = 0;
	}
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg,IDC_MAPPER105_J0,(Mapper.MaxCount & 0x02000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER105_J1,(Mapper.MaxCount & 0x04000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER105_J2,(Mapper.MaxCount & 0x08000000) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER105_J3,(Mapper.MaxCount & 0x10000000) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Mapper.MaxCount = 0;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J0) == BST_CHECKED)
					Mapper.MaxCount |= 0x02000000;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J1) == BST_CHECKED)
					Mapper.MaxCount |= 0x04000000;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J2) == BST_CHECKED)
					Mapper.MaxCount |= 0x08000000;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J3) == BST_CHECKED)
					Mapper.MaxCount |= 0x10000000;
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
	Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER105),hWnd,(DLGPROC)ConfigProc);
	if (Mapper.ConfigWindow)
		SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC1_Destroy();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
	Mapper.ConfigWindow = NULL;
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	if (IsHardReset)
		Mapper.MaxCount = 0x04000000;
	Mapper.Counter = 0;
	Mapper.CounterEnabled = 0;
	Mapper.InitState = 0;
	Mapper.ConfigWindow = NULL;

	MMC1_Init(Sync);
}

CTMapperInfo	MapperInfo_105 =
{
	105,
	NULL,
	"Nintendo World Championship",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	Config
};