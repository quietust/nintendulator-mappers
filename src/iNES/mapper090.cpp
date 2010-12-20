/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"resource.h"

namespace
{
uint8 IRQenabled, IRQmode, IRQcounterL, IRQcounterH, IRQxor;
uint16 IRQaddr;
uint8 Mul1, Mul2;
uint8 BankMode, Mirror, MirBank, ExtBank;
uint8 PRGbanks[4];
uint16_n CHRbanks[8];
uint16_n Nametables[4];
uint8 treg;
uint8 Jumper;
HWND ConfigWindow;
uint8 ConfigCmd;
FCPUWrite _CPUWrite[0x10];
FPPURead _PPURead[0x10];

uint8 ReverseBits (uint8 bits)
{
	uint8 out = 0;
	if (bits & 0x01) out |= 0x40;
	if (bits & 0x02) out |= 0x20;
	if (bits & 0x04) out |= 0x10;
	if (bits & 0x08) out |= 0x08;
	if (bits & 0x10) out |= 0x04;
	if (bits & 0x20) out |= 0x02;
	if (bits & 0x40) out |= 0x01;
	return out;
}

void	SyncPRG (void)
{
	if (!(BankMode & 0x80))
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
	switch (BankMode & 0x3)
	{
	case 0:	if (BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6, (PRGbanks[3] << 2) | 3);
		EMU->SetPRG_ROM32(0x8, (BankMode & 0x4) ? PRGbanks[3] : 0x7F);
		break;
	case 1:	if (BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6, (PRGbanks[3] << 1) | 1);
		EMU->SetPRG_ROM16(0x8, PRGbanks[1]);
		EMU->SetPRG_ROM16(0xC, (BankMode & 0x4) ? PRGbanks[3] : 0x7F);
		break;
	case 2:	if (BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6, PRGbanks[3]);
		EMU->SetPRG_ROM8(0x8, PRGbanks[0]);
		EMU->SetPRG_ROM8(0xA, PRGbanks[1]);
		EMU->SetPRG_ROM8(0xC, PRGbanks[2]);
		EMU->SetPRG_ROM8(0xE, (BankMode & 0x4) ? PRGbanks[3] : 0x7F);
		break;
	case 3:	if (BankMode & 0x80)
			EMU->SetPRG_ROM8(0x6, ReverseBits(PRGbanks[3]));
		EMU->SetPRG_ROM8(0x8, ReverseBits(PRGbanks[0]));
		EMU->SetPRG_ROM8(0xA, ReverseBits(PRGbanks[1]));
		EMU->SetPRG_ROM8(0xC, ReverseBits(PRGbanks[2]));
		EMU->SetPRG_ROM8(0xE, (BankMode & 0x4) ? ReverseBits(PRGbanks[3]) : 0x7F);
		break;
	}
}

void	SyncCHR (void)
{
	switch ((BankMode & 0x18) >> 3)
	{
	case 0:	EMU->SetCHR_ROM8(0, (ExtBank & 0x20) ? CHRbanks[0].s0 : (CHRbanks[0].b0 | ((ExtBank & 0x1F) << 8)));	break;
	case 1:	EMU->SetCHR_ROM4(0, (ExtBank & 0x20) ? CHRbanks[0].s0 : (CHRbanks[0].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM4(4, (ExtBank & 0x20) ? CHRbanks[4].s0 : (CHRbanks[4].b0 | ((ExtBank & 0x1F) << 8)));	break;
	case 2:	EMU->SetCHR_ROM2(0, (ExtBank & 0x20) ? CHRbanks[0].s0 : (CHRbanks[0].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(2, (ExtBank & 0x20) ? CHRbanks[2].s0 : (CHRbanks[2].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(4, (ExtBank & 0x20) ? CHRbanks[4].s0 : (CHRbanks[4].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM2(6, (ExtBank & 0x20) ? CHRbanks[6].s0 : (CHRbanks[6].b0 | ((ExtBank & 0x1F) << 8)));	break;
	case 3:	EMU->SetCHR_ROM1(0, (ExtBank & 0x20) ? CHRbanks[0].s0 : (CHRbanks[0].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(1, (ExtBank & 0x20) ? CHRbanks[1].s0 : (CHRbanks[1].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(2, (ExtBank & 0x20) ? CHRbanks[2].s0 : (CHRbanks[2].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(3, (ExtBank & 0x20) ? CHRbanks[3].s0 : (CHRbanks[3].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(4, (ExtBank & 0x20) ? CHRbanks[4].s0 : (CHRbanks[4].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(5, (ExtBank & 0x20) ? CHRbanks[5].s0 : (CHRbanks[5].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(6, (ExtBank & 0x20) ? CHRbanks[6].s0 : (CHRbanks[6].b0 | ((ExtBank & 0x1F) << 8)));
		EMU->SetCHR_ROM1(7, (ExtBank & 0x20) ? CHRbanks[7].s0 : (CHRbanks[7].b0 | ((ExtBank & 0x1F) << 8)));	break;
	}
}

void	SyncNametables (void)
{
	if ((BankMode & 0x20) && (Jumper & 0x01))
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			if ((BankMode & 0x40) || ((Nametables[i].b0 ^ MirBank) & 0x80))
			{
				EMU->SetCHR_ROM1(0x8 | i, (ExtBank & 0x20) ? Nametables[i].s0 : (Nametables[i].b0 | ((ExtBank & 0x1F) << 8)));
				EMU->SetCHR_ROM1(0xC | i, (ExtBank & 0x20) ? Nametables[i].s0 : (Nametables[i].b0 | ((ExtBank & 0x1F) << 8)));
			}
			else
			{
				EMU->SetCHR_NT1(0x8 | i, Nametables[i].b0 & 1);
				EMU->SetCHR_NT1(0xC | i, Nametables[i].b0 & 1);
			}
		}
	}
	else
	{
		switch (Mirror & 3)
		{
		case 0:	EMU->Mirror_V();	break;
		case 1:	EMU->Mirror_H();	break;
		case 2:	EMU->Mirror_S0();	break;
		case 3:	EMU->Mirror_S1();	break;
		}
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, PRGbanks[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_WORD(mode, offset, data, CHRbanks[i].s0);
	for (int i = 0; i < 4; i++)
		SAVELOAD_WORD(mode, offset, data, Nametables[i].s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQmode);
	SAVELOAD_BYTE(mode, offset, data, IRQcounterL);
	SAVELOAD_BYTE(mode, offset, data, IRQcounterH);
	SAVELOAD_BYTE(mode, offset, data, IRQxor);
	SAVELOAD_WORD(mode, offset, data, IRQaddr);
	SAVELOAD_BYTE(mode, offset, data, BankMode);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	SAVELOAD_BYTE(mode, offset, data, MirBank);
	SAVELOAD_BYTE(mode, offset, data, ExtBank);
	SAVELOAD_BYTE(mode, offset, data, Mul1);
	SAVELOAD_BYTE(mode, offset, data, Mul2);
	SAVELOAD_BYTE(mode, offset, data, Jumper);
	SAVELOAD_BYTE(mode, offset, data, treg);
	if (mode == STATE_LOAD)
	{
		SyncPRG();
		SyncCHR();
		SyncNametables();
	}
	return offset;
}

void	IRQcount (void)
{
	static unsigned char mask;
	if (IRQmode & 0x4)
		mask = 0x7;
	else	mask = 0xFF;
	if ((IRQmode & 0xC0) == 0x80)
	{
		IRQcounterL--;
		if ((IRQcounterL & mask) == mask)
		{
			IRQcounterH--;
			if (IRQcounterH == 0xFF)
				EMU->SetIRQ(0);
		}
	}
	if ((IRQmode & 0xC0) == 0x40)
	{
		IRQcounterL++;
		if (!(IRQcounterL & mask))
		{
			IRQcounterH++;
			if (!IRQcounterH)
				EMU->SetIRQ(0);
		}
	}
}
void	MAPINT	CPUCycle (void)
{
	if (IRQenabled && ((IRQmode & 0x3) == 0))
		IRQcount();
}
void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (IRQenabled && ((IRQmode & 0x3) == 1))
	{
		if (!(IRQaddr & 0x1000) && (Addr & 0x1000))
			IRQcount();
		IRQaddr = Addr;
	}
}
void	MAPINT	CPUWrite (int Bank, int Addr, int Val)
{
	if (IRQenabled && ((IRQmode & 0x3) == 2))
		IRQcount();
	_CPUWrite[Bank](Bank, Addr, Val);
}
int	MAPINT	PPURead (int Bank, int Addr)
{
	if (IRQenabled && ((IRQmode & 0x3) == 3))
		IRQcount();
	return _PPURead[Bank](Bank, Addr);
}

int	MAPINT	Read5 (int Bank, int Addr)
{
	switch (Addr & 0x803)
	{
	case 0x000:return (Jumper & 0xC0) |
			(*EMU->OpenBus & 0x3F);			break;
	case 0x800:return (Mul1 * Mul2) & 0xFF;	break;
	case 0x801:return (Mul1 * Mul2) >> 8;	break;
	case 0x803:return treg;				break;
	}
	return -1;
}

void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	switch (Addr & 0x803)
	{
	case 0x800:Mul1 = Val;	break;
	case 0x801:Mul2 = Val;	break;
	case 0x803:treg = Val;	break;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRGbanks[Addr & 3] = Val;
	SyncPRG();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	CHRbanks[Addr & 7].b0 = Val;
	SyncCHR();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	CHRbanks[Addr & 7].b1 = Val;
	SyncCHR();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 4)
		Nametables[Addr & 3].b1 = Val;
	else	Nametables[Addr & 3].b0 = Val;
	SyncNametables();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	/* usually unused */	break;
	case 1:	IRQmode = Val;
		switch (Val & 3)
		{
		case 0:	EMU->DbgOut(_T("Mapper 90 IRQ counter set to CPU M2"));		break;
		case 1:	EMU->DbgOut(_T("Mapper 90 IRQ counter set to PPU A12"));	break;
		case 2:	EMU->DbgOut(_T("Mapper 90 IRQ counter set to CPU R/W"));	break;
		case 3:	EMU->DbgOut(_T("Mapper 90 IRQ counter set to PPU /RD"));	break;
		}	break;
	case 2:	IRQenabled = 0;
		EMU->SetIRQ(1);		break;
	case 3:	IRQenabled = 1;	break;
	case 4:	IRQcounterL = Val ^ IRQxor;
					break;
	case 5:	IRQcounterH = Val ^ IRQxor;
					break;
	case 6:	IRQxor = Val;	break;
	case 7:	/* usually unused */	break;
	}
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	BankMode = Val;
		SyncPRG();
		SyncCHR();
		SyncNametables();	break;
	case 1:	Mirror = Val;
		SyncNametables();	break;
	case 2: MirBank = Val;
		SyncNametables();	break;
	case 3:	ExtBank = Val;
		SyncCHR();		break;
	}
}

LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_MAPPER90_J0, (Jumper & 0x40) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_MAPPER90_J1, (Jumper & 0x80) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_MAPPER90_MIR, (Jumper & 0x01) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER90_J0) == BST_CHECKED)
					ConfigCmd |= 0x01;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER90_J1) == BST_CHECKED)
					ConfigCmd |= 0x02;
				if (IsDlgButtonChecked(hDlg, IDC_MAPPER90_MIR) == BST_CHECKED)
					ConfigCmd |= 0x04;
				MessageBox(hDlg, _T("Please perform a SOFT RESET for this to take effect!"), _T("INES.DLL"), MB_OK);
			case IDCANCEL:
				DestroyWindow(hDlg);
				ConfigWindow = NULL;
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hDlg);
			ConfigWindow = NULL;
			return TRUE;		break;
	}
	return FALSE;
}

unsigned char	MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (ConfigWindow)
				break;
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER90), hWnd, (DLGPROC)ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
		{
			Jumper = 0;
			if (data & 0x01)
				Jumper |= 0x40;
			if (data & 0x02)
				Jumper |= 0x80;
			if (data & 0x04)
				Jumper |= 0x01;
			SyncNametables();
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

void	MAPINT	Load (void)
{
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUReadHandler(0x5, Read5);
	EMU->SetCPUWriteHandler(0x5, Write5);
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	
	ConfigCmd = 0;
	if (ResetType == RESET_HARD)
	{
		IRQenabled = IRQmode = IRQcounterL = IRQcounterH = IRQxor = 0;
		IRQaddr = 0;
		BankMode = Mirror = MirBank = ExtBank = 0;
		for (int x = 0; x < 8; x++)
			CHRbanks[x].s0 = 0;
		for (int x = 0; x < 4; x++)
			Nametables[x].s0 = 0;
		for (int x = 0; x < 4; x++)
			PRGbanks[x] = 0;
		Mul1 = Mul2 = 0;
		Jumper = 0;
	}
	for (int x = 0; x < 16; x++)
	{
		_CPUWrite[x] = EMU->GetCPUWriteHandler(x);
		EMU->SetCPUWriteHandler(x, CPUWrite);
		_PPURead[x] = EMU->GetPPUReadHandler(x);
		EMU->SetPPUReadHandler(x, PPURead);
	}

	SyncPRG();
	SyncCHR();
	SyncNametables();
}
void	MAPINT	Unload (void)
{
	if (ConfigWindow)
		DestroyWindow(ConfigWindow);
}

uint8 MapperNum = 90;
} // namespace

const MapperInfo MapperInfo_090 =
{
	&MapperNum,
	_T("Mapper 90"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	NULL,
	Config
};