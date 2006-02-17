#include	"..\DLL\d_iNES.h"
#include	"resource.h"

static	struct
{
	FCPUWrite Write4;
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG;
	u8 Title;
	HWND ConfigWindow;
	u8 ConfigCmd;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM4(0x5,(8 << 1) | Mapper.Title);
	EMU->SetPRG_ROM8(0x6,2);
	EMU->SetPRG_ROM8(0x8,1);
	EMU->SetPRG_ROM8(0xA,0);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG);
	EMU->SetPRG_ROM8(0xE,9);
	EMU->SetCHR_ROM8(0,0);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		Mapper.IRQcounter.s0++;
		if (Mapper.IRQcounter.s0 >= 4096)
			EMU->SetIRQ(0);
	}
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	const u8 PRGbanks[8] = {4,3,4,4,4,7,5,6};
	u16 Loc = (Bank << 12) | Addr;
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	if ((Loc & 0x71FF) == 0x4022)
	{
		Mapper.PRG = PRGbanks[Val & 7];
		Sync();
	}
	if ((Loc & 0x71FF) == 0x0122)
	{
		if (Val & 3)
			Mapper.IRQenabled = 1;
		else
		{
			Mapper.IRQcounter.s0 = 0;
			Mapper.IRQenabled = 0;
			EMU->SetIRQ(1);
		}
	}
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			if (Mapper.Title == 1)
				CheckRadioButton(hDlg,IDC_MAPPER43_BLANK,IDC_MAPPER43_LOGO,IDC_MAPPER43_BLANK);
			else	CheckRadioButton(hDlg,IDC_MAPPER43_BLANK,IDC_MAPPER43_LOGO,IDC_MAPPER43_LOGO);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Mapper.ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER43_BLANK) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x01;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER43_LOGO) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x00;
				else	MessageBox(hWnd,"Impossible - neither radio button checked!","INES.DLL",MB_OK);
				MessageBox(hWnd,"Please perform a SOFT reset for this to take effect!","INES.DLL",MB_OK);
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

static	unsigned char	_MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER43),hWnd,(DLGPROC)ConfigProc);
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
			Mapper.Title = data & 0x1;
			Sync();
		}
		Mapper.ConfigCmd = 0;
		break;
	}
	return 0;
}

static	void	_MAPINT	Load (void)
{
	Mapper.ConfigWindow = NULL;
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUReadHandler(0x5,EMU->GetCPUReadHandler(0x8));

	for (x = 0x4; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
	{
		Mapper.Title = 1;
		Mapper.PRG = 0;
		Mapper.IRQenabled = 1;
		Mapper.IRQcounter.s0 = 0;
	}

	Mapper.ConfigCmd = 0;
	Sync();
}
static	void	_MAPINT	Unload (void)
{
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
}

static	u8 MapperNum = 43;
CTMapperInfo	MapperInfo_043 =
{
	&MapperNum,
	"SMB2j (LF36)",
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
