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

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
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

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	const u8 PRGbanks[8] = {4,3,4,4,4,7,5,6};
	u16 Loc = (Bank << 12) | Where;
	if (Bank == 4)
		Mapper.Write4(Bank,Where,What);
	if ((Loc & 0x71FF) == 0x4022)
	{
		Mapper.PRG = PRGbanks[What & 7];
		Sync();
	}
	if ((Loc & 0x71FF) == 0x0122)
	{
		if (What & 3)
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
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER43_BLANK) == BST_CHECKED)
					Mapper.Title = 1;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER43_LOGO) == BST_CHECKED)
					Mapper.Title = 0;
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

static	void	_MAPINT	Config (void)
{
	Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER43),hWnd,(DLGPROC)ConfigProc);
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
	u8 x;
	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUReadHandler(0x5,EMU->GetCPUReadHandler(0x8));

	for (x = 0x4; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (IsHardReset)
		Mapper.Title = 1;
	Mapper.PRG = 0;
	Mapper.IRQenabled = 1;
	Mapper.IRQcounter.s0 = 0;

	Mapper.ConfigWindow = NULL;

	Sync();
}

CTMapperInfo	MapperInfo_043 =
{
	43,
	NULL,
	"SMB2j (LF36)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	Config
};
