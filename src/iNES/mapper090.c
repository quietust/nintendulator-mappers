#include	"..\DLL\d_iNES.h"
#include	"resource.h"

static	struct
{
	u8 IRQenabled, IRQmode, IRQcounterL, IRQcounterH, IRQxor;
	u16 IRQaddr;
	u8 Mul1, Mul2;
	u8 BankMode, Mirror, MirBank, ExtBank;
	u8 PRGbanks[4];
	u16_n CHRbanks[8];
	u16_n Nametables[4];
	u8 treg;
	u8 Jumper;
	HWND ConfigWindow;
	u8 ConfigCmd;
}	Mapper;

static	u8 ReverseBits (u8 bits)
{
	u8 out = 0;
	if (bits & 0x01) out |= 0x40;
	if (bits & 0x02) out |= 0x20;
	if (bits & 0x04) out |= 0x10;
	if (bits & 0x08) out |= 0x08;
	if (bits & 0x10) out |= 0x04;
	if (bits & 0x20) out |= 0x02;
	if (bits & 0x40) out |= 0x01;
	return out;
}

static	void	SyncPRG (void)
{
	if (!(Mapper.BankMode & 0x80))
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
	switch (Mapper.BankMode & 0x3)
	{
	case 0:	if (Mapper.BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6,(Mapper.PRGbanks[3] << 2) | 3);
		EMU->SetPRG_ROM32(0x8,(Mapper.BankMode & 0x4) ? Mapper.PRGbanks[3] : 0x7F);
		break;
	case 1:	if (Mapper.BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6,(Mapper.PRGbanks[3] << 1) | 1);
		EMU->SetPRG_ROM16(0x8,Mapper.PRGbanks[1]);
		EMU->SetPRG_ROM16(0xC,(Mapper.BankMode & 0x4) ? Mapper.PRGbanks[3] : 0x7F);
		break;
	case 2:	if (Mapper.BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6,Mapper.PRGbanks[3]);
		EMU->SetPRG_ROM8(0x8,Mapper.PRGbanks[0]);
		EMU->SetPRG_ROM8(0xA,Mapper.PRGbanks[1]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRGbanks[2]);
		EMU->SetPRG_ROM8(0xE,(Mapper.BankMode & 0x4) ? Mapper.PRGbanks[3] : 0x7F);
		break;
	case 3:	if (Mapper.BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6,ReverseBits(Mapper.PRGbanks[3]));
		EMU->SetPRG_ROM8(0x8,ReverseBits(Mapper.PRGbanks[0]));
		EMU->SetPRG_ROM8(0xA,ReverseBits(Mapper.PRGbanks[1]));
		EMU->SetPRG_ROM8(0xC,ReverseBits(Mapper.PRGbanks[2]));
		EMU->SetPRG_ROM8(0xE,(Mapper.BankMode & 0x4) ? ReverseBits(Mapper.PRGbanks[3]) : 0x7F);
		break;
	}
}

static	void	SyncCHR (void)
{
	switch ((Mapper.BankMode & 0x18) >> 3)
	{
	case 0:	EMU->SetCHR_ROM8(0,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[0].s0 : (Mapper.CHRbanks[0].b0 | ((Mapper.ExtBank & 0x1F) << 8)));	break;
	case 1:	EMU->SetCHR_ROM4(0,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[0].s0 : (Mapper.CHRbanks[0].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM4(4,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[4].s0 : (Mapper.CHRbanks[4].b0 | ((Mapper.ExtBank & 0x1F) << 8)));	break;
	case 2:	EMU->SetCHR_ROM2(0,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[0].s0 : (Mapper.CHRbanks[0].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(2,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[2].s0 : (Mapper.CHRbanks[2].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(4,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[4].s0 : (Mapper.CHRbanks[4].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(6,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[6].s0 : (Mapper.CHRbanks[6].b0 | ((Mapper.ExtBank & 0x1F) << 8)));	break;
	case 3:	EMU->SetCHR_ROM1(0,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[0].s0 : (Mapper.CHRbanks[0].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(1,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[1].s0 : (Mapper.CHRbanks[1].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(2,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[2].s0 : (Mapper.CHRbanks[2].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(3,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[3].s0 : (Mapper.CHRbanks[3].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(4,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[4].s0 : (Mapper.CHRbanks[4].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(5,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[5].s0 : (Mapper.CHRbanks[5].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(6,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[6].s0 : (Mapper.CHRbanks[6].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(7,(Mapper.ExtBank & 0x20) ? Mapper.CHRbanks[7].s0 : (Mapper.CHRbanks[7].b0 | ((Mapper.ExtBank & 0x1F) << 8)));	break;
	}
}

static	void	SyncNametables (void)
{
	if ((Mapper.BankMode & 0x20) && (Mapper.Jumper & 0x01))
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			if ((Mapper.BankMode & 0x40) || ((Mapper.Nametables[i].b0 ^ Mapper.MirBank) & 0x80))
			{
				EMU->SetCHR_ROM1(0x8|i,(Mapper.ExtBank & 0x20) ? Mapper.Nametables[i].s0 : (Mapper.Nametables[i].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
				EMU->SetCHR_ROM1(0xC|i,(Mapper.ExtBank & 0x20) ? Mapper.Nametables[i].s0 : (Mapper.Nametables[i].b0 | ((Mapper.ExtBank & 0x1F) << 8)));
			}
			else
			{
				EMU->SetCHR_NT1(0x8|i,Mapper.Nametables[i].b0 & 1);
				EMU->SetCHR_NT1(0xC|i,Mapper.Nametables[i].b0 & 1);
			}
		}
	}
	else
	{
		switch (Mapper.Mirror & 3)
		{
		case 0:	EMU->Mirror_V();	break;
		case 1:	EMU->Mirror_H();	break;
		case 2:	EMU->Mirror_S0();	break;
		case 3:	EMU->Mirror_S1();	break;
		}
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRGbanks[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_WORD(mode,x,data,Mapper.CHRbanks[i].s0)
	for (i = 0; i < 4; i++)
		SAVELOAD_WORD(mode,x,data,Mapper.Nametables[i].s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQmode)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounterL)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounterH)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQxor)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQaddr)
	SAVELOAD_BYTE(mode,x,data,Mapper.BankMode)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	SAVELOAD_BYTE(mode,x,data,Mapper.MirBank)
	SAVELOAD_BYTE(mode,x,data,Mapper.ExtBank)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mul1)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mul2)
	SAVELOAD_BYTE(mode,x,data,Mapper.Jumper)
	SAVELOAD_BYTE(mode,x,data,Mapper.treg)
	if (mode == STATE_LOAD)
	{
		SyncPRG();
		SyncCHR();
		SyncNametables();
	}
	return x;
}

static	void	IRQcount (void)
{
	static unsigned char mask;
	if (Mapper.IRQmode & 0x4)
		mask = 0x7;
	else	mask = 0xFF;
	if ((Mapper.IRQmode & 0xC0) == 0x80)
	{
		Mapper.IRQcounterL--;
		if ((Mapper.IRQcounterL & mask) == mask)
		{
			Mapper.IRQcounterH--;
			if (Mapper.IRQcounterH == 0xFF)
				EMU->SetIRQ(0);
		}
	}
	if ((Mapper.IRQmode & 0xC0) == 0x40)
	{
		Mapper.IRQcounterL++;
		if (!(Mapper.IRQcounterL & mask))
		{
			Mapper.IRQcounterH++;
			if (!Mapper.IRQcounterH)
				EMU->SetIRQ(0);
		}
	}
}
static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled && ((Mapper.IRQmode & 0x3) == 0))
		IRQcount();
}
static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (Mapper.IRQenabled && ((Mapper.IRQmode & 0x3) == 1))
	{
		if (!(Mapper.IRQaddr & 0x1000) && (Addr & 0x1000))
			IRQcount();
		Mapper.IRQaddr = Addr;
	}
}

static	int	_MAPINT	Read5 (int Bank, int Addr)
{
	switch (Addr & 0x803)
	{
	case 0x000:return (Mapper.Jumper & 0xC0) |
			(EMU->GetCPUReadHandler(4)(4,0) & 0x3F);break;
	case 0x800:return (Mapper.Mul1 * Mapper.Mul2) & 0xFF;	break;
	case 0x801:return (Mapper.Mul1 * Mapper.Mul2) >> 8;	break;
	case 0x803:return Mapper.treg;				break;
	}
	return -1;
}

static	void	_MAPINT	Write5 (int Bank, int Addr, int Val)
{
	switch (Addr & 0x803)
	{
	case 0x800:Mapper.Mul1 = Val;	break;
	case 0x801:Mapper.Mul2 = Val;	break;
	case 0x803:Mapper.treg = Val;	break;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.PRGbanks[Addr & 3] = Val;
	SyncPRG();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.CHRbanks[Addr & 7].b0 = Val;
	SyncCHR();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.CHRbanks[Addr & 7].b1 = Val;
	SyncCHR();
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 4)
		Mapper.Nametables[Addr & 3].b1 = Val;
	else	Mapper.Nametables[Addr & 3].b0 = Val;
	SyncNametables();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	/* usually unused */	break;
	case 1:	Mapper.IRQmode = Val;
		switch (Val & 3)
		{
		case 0:	EMU->DbgOut("Mapper 90 IRQ counter set to CPU M2");	break;
		case 1:	EMU->DbgOut("Mapper 90 IRQ counter set to PPU A12");	break;
		case 2:	EMU->DbgOut("Mapper 90 IRQ counter set to CPU R/W");	break;
		case 3:	EMU->DbgOut("Mapper 90 IRQ counter set to PPU /RD");	break;
		}	break;
	case 2:	Mapper.IRQenabled = 0;
		EMU->SetIRQ(1);		break;
	case 3:	Mapper.IRQenabled = 1;	break;
	case 4:	Mapper.IRQcounterL = Val ^ Mapper.IRQxor;
					break;
	case 5:	Mapper.IRQcounterH = Val ^ Mapper.IRQxor;
					break;
	case 6:	Mapper.IRQxor = Val;	break;
	case 7:	/* usually unused */	break;
	}
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	Mapper.BankMode = Val;
		SyncPRG();
		SyncCHR();
		SyncNametables();	break;
	case 1:	Mapper.Mirror = Val;
		SyncNametables();	break;
	case 2: Mapper.MirBank = Val;
		SyncNametables();	break;
	case 3:	Mapper.ExtBank = Val;
		SyncCHR();		break;
	}
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg,IDC_MAPPER90_J0,(Mapper.Jumper & 0x40) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER90_J1,(Mapper.Jumper & 0x80) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg,IDC_MAPPER90_MIR,(Mapper.Jumper & 0x01) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Mapper.ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER90_J0) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER90_J1) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x02;
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER90_MIR) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x04;
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

static	unsigned char	_MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER90),hWnd,(DLGPROC)ConfigProc);
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
			Mapper.Jumper = 0;
			if (data & 0x01)
				Mapper.Jumper |= 0x40;
			if (data & 0x02)
				Mapper.Jumper |= 0x80;
			if (data & 0x04)
				Mapper.Jumper |= 0x01;
			SyncNametables();
		}
		Mapper.ConfigCmd = 0;
		break;
	}
	return 0;
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_InitROM();

	EMU->SetCPUReadHandler(0x5,Read5);
	EMU->SetCPUWriteHandler(0x5,Write5);
	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	
	Mapper.ConfigWindow = NULL;
	Mapper.ConfigCmd = 0;
	if (ResetType == RESET_HARD)
	{
		Mapper.IRQenabled = Mapper.IRQmode = Mapper.IRQcounterL = Mapper.IRQcounterH = Mapper.IRQxor = 0;
		Mapper.IRQaddr = 0;
		Mapper.BankMode = Mapper.Mirror = Mapper.MirBank = Mapper.ExtBank = 0;
		for (x = 0; x < 8; x++)	Mapper.CHRbanks[x].s0 = 0;
		for (x = 0; x < 4; x++)	Mapper.Nametables[x].s0 = 0;
		for (x = 0; x < 4; x++)	Mapper.PRGbanks[x] = 0;
		Mapper.Mul1 = Mapper.Mul2 = 0;
		Mapper.Jumper = 0;
	}

	SyncPRG();
	SyncCHR();
	SyncNametables();
}

static	u8 MapperNum = 90;
CTMapperInfo	MapperInfo_090 =
{
	&MapperNum,
	"Mapper 90",
	COMPAT_FULL,
	Reset,
	NULL,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	NULL,
	Config
};