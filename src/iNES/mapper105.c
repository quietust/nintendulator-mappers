#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"
#include	"resource.h"

static	struct
{
	u32 Counter, MaxCount;
	u8 CounterEnabled;
	u8 InitState;
	HWND ConfigWindow;
	u8 ConfigCmd;
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

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
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
		u32 SecondsLeft = ((Mapper.MaxCount | 0x20000000) - Mapper.Counter) / 1789773;
		EMU->StatusOut("Time left: %02i:%02i",SecondsLeft / 60,SecondsLeft % 60);
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
				Mapper.ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J0) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J1) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x02;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J2) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x04;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER105_J3) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x08;
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

static	unsigned char	_MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER105),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return Mapper.ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
			Mapper.MaxCount = (data & 0xF) << 25;
		Mapper.ConfigCmd = 0;
		break;
	}
	return 0;
}

static	void	_MAPINT	Load (void)
{
	MMC1_Load(Sync);
	Mapper.ConfigWindow = NULL;
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Mapper.MaxCount = 0x04000000;

	Mapper.Counter = 0;
	Mapper.CounterEnabled = 0;
	Mapper.InitState = 0;
	Mapper.ConfigCmd = 0;

	MMC1_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC1_Unload();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
}

static	u8 MapperNum = 105;
CTMapperInfo	MapperInfo_105 =
{
	&MapperNum,
	"Nintendo World Championship",
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