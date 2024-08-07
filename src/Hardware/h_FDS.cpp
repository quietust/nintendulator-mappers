/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_FDS.h"
#include	"Sound\s_FDS.h"
#include	"..\FDS\resource.h"
#include	<stdio.h>

namespace FDS
{
FCPURead _Read;
FCPUWrite _Write;
uint8_t DiskNum;
uint16_t IRQcounter;
uint16_n IRQlatch;
uint8_t IRQenabled;
uint8_t IOenable;
uint8_t IOcontrol;
uint8_t IOstatus;
int BytePtr;
uint8_t WriteSkip;
uint8_t DiskIRQ;
uint8_t Mirror;
HWND ConfigWindow;
uint8_t ConfigCmd;

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, DiskNum);
	SAVELOAD_WORD(mode, offset, data, IRQcounter);
	SAVELOAD_WORD(mode, offset, data, IRQlatch.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IOenable);
	SAVELOAD_BYTE(mode, offset, data, IOcontrol);
	SAVELOAD_BYTE(mode, offset, data, IOstatus);
	SAVELOAD_WORD(mode, offset, data, BytePtr);
	SAVELOAD_BYTE(mode, offset, data, WriteSkip);
	SAVELOAD_BYTE(mode, offset, data, DiskIRQ);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	CheckSave(offset = FDSsound::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
	{
		if (Mirror)
			EMU->Mirror_A11();
		else	EMU->Mirror_A10();
	}
	return offset;
}

#define	IRQ_TIMER	0x01
#define	IRQ_DISK	0x02

inline	void	StartIRQ (uint8_t flag)
{
	IOstatus |= flag;
	EMU->SetIRQ(0);
}

inline	void	EndIRQ (uint8_t flag)
{
	IOstatus &= ~flag;
	if (!IOstatus)
		EMU->SetIRQ(1);
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled & 0x2) && (IRQcounter) && (!--IRQcounter))
	{
		if (IRQenabled & 0x1)
			IRQcounter = IRQlatch.s0;
		else	IRQenabled &= 1;
		StartIRQ(IRQ_TIMER);
	}
	if ((DiskIRQ) && (!--DiskIRQ) && (IOcontrol & 0x80))
		StartIRQ(IRQ_DISK);
}

#define	DISKIRQ_SHORT	150
#define	DISKIRQ_LONG	200

int	MAPINT	Read (int Bank, int Addr)
{
	uint8_t result;
	static uint8_t DiskVal = 0;
	if ((Addr & 0xFFF) < 0x18)
		return _Read(Bank, Addr);
	switch (Addr & 0xFFF)
	{
	case 0x30:	result = IOstatus;
			EndIRQ(0xFF);	// everything
			return result;		break;
	case 0x31:	if (DiskNum == 0xFF)
				return DiskVal;
			EMU->SetPRG_ROM4(0x5, (DiskNum << 4) | ((BytePtr >> 12) & 0xF));
			DiskVal = EMU->GetPRG_Ptr4(0x5)[BytePtr & 0xFFF];
			EMU->SetPRG_OB4(0x5);
//			if (IOcontrol & 0x01)
			{
				if (BytePtr < 64999)
					BytePtr++;
				if ((BytePtr & 0xFF) == 0)
					EMU->StatusOut(_T("%i R/S"), BytePtr);
				DiskIRQ = DISKIRQ_SHORT;
				EndIRQ(IRQ_DISK);
			}
			return DiskVal;			break;
	case 0x32:	result = 0x40;
			if (DiskNum == 0xFF)
				result |= 0x05;
			if ((DiskNum == 0xFF) || (!(IOcontrol & 0x01)) || (IOcontrol & 0x02))
				result |= 0x02;
			return result;			break;
	case 0x33:	return 0x80;			break;
	}
	return FDSsound::Read((Bank << 12) | Addr);
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if ((Addr & 0xFFF) < 0x18)
		_Write(Bank, Addr, Val);
	FDSsound::Write((Bank << 12) | Addr, Val);
	switch (Addr & 0xFFF)
	{
	case 0x20:	IRQlatch.b0 = Val;
			EndIRQ(IRQ_TIMER);		break;
	case 0x21:	IRQlatch.b1 = Val;
			EndIRQ(IRQ_TIMER);		break;
	case 0x22:	IRQenabled = Val & 0x3;
			IRQcounter = IRQlatch.s0;
			EndIRQ(IRQ_TIMER);		break;
	case 0x23:	IOenable = Val;		break;
	case 0x24:	if ((DiskNum != 0xFF) && (!(IOcontrol & 0x04)) && (IOenable & 0x1))
			{
				if ((BytePtr >= 0) && (BytePtr < 65000))
				{
					if (WriteSkip) WriteSkip--;
					else if (BytePtr >= 2)
					{
						EMU->StatusOut(_T("%i W"), BytePtr);
						BytePtr -= 2;
						EMU->SetPRG_ROM4(0x5, (DiskNum << 4) | ((BytePtr >> 12) & 0xF));
						EMU->GetPRG_Ptr4(0x5)[BytePtr & 0xFFF] = Val;
						EMU->SetPRG_OB4(0x5);
						BytePtr += 2;
					}
				}
			}				break;
	case 0x25:	EndIRQ(IRQ_DISK);
			Mirror = Val & 0x8;
			if (Mirror)
				EMU->Mirror_A11();
			else	EMU->Mirror_A10();
			if (DiskNum == 0xFF)	break;
			if (!(Val & 0x40))
			{
				if ((IOcontrol & 0x40) && (!(Val & 0x10)))
				{
					EMU->StatusOut(_T("%i S (Reverse)"), BytePtr);
					BytePtr -= 2;
					DiskIRQ = DISKIRQ_LONG;
				}
				if (BytePtr < 0)
					BytePtr = 0;
			}
			if (!(Val & 0x04))
				WriteSkip = 2;
			if (Val & 0x02)
			{
				EMU->StatusOut(_T("%i S (Zero)"), BytePtr);
				BytePtr = 0;
				DiskIRQ = DISKIRQ_LONG;
			}
			if (Val & 0x40)
				DiskIRQ = DISKIRQ_LONG;
			IOcontrol = Val;
			break;
	}
}

INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[256];
	switch (message)
	{
	case WM_INITDIALOG:
		for (int i = 0; i < ROM->FDS_NumSides; i++)
		{
			_stprintf(buf, _T("Disk %i Side %s"), (i >> 1)+1, (i & 1) ? _T("B") : _T("A"));
			SendDlgItemMessage(hDlg, IDC_FDS_DISKSEL, CB_ADDSTRING, 0, (LPARAM)buf);
		}
		if (DiskNum == 0xFF)
		{
			SendDlgItemMessage(hDlg, IDC_FDS_DISKSEL, CB_SETCURSEL, 0, 0);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_DISKSEL), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_INSERT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_EJECT), FALSE);
		}
		else
		{
			SendDlgItemMessage(hDlg, IDC_FDS_DISKSEL, CB_SETCURSEL, DiskNum, 0);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_DISKSEL), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_INSERT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_FDS_EJECT), TRUE);
		}
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_FDS_EJECT:
			ConfigCmd = 0xFF;
			return TRUE;
		case IDC_FDS_INSERT:
			ConfigCmd = (uint8_t)SendDlgItemMessage(hDlg, IDC_FDS_DISKSEL, CB_GETCURSEL, 0, 0) + 1;
			return TRUE;
		case IDCANCEL:
			ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;
		}
		break;
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_FDS), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return ConfigCmd;
		break;
	case CFG_CMD:
		if (data)
		{
			if (data == 0xFF)
			{
				DiskNum = 0xFF;
				EMU->StatusOut(_T("Disk ejected!"));
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_DISKSEL), TRUE);
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_INSERT), TRUE);
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_EJECT), FALSE);
			}
			else
			{
				DiskNum = data - 1;
				EMU->StatusOut(_T("Disk %i side %s inserted!"), (DiskNum >> 1) + 1, (DiskNum & 1) ? _T("B") : _T("A"));
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_DISKSEL), FALSE);
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_INSERT), FALSE);
				EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_EJECT), TRUE);
			}
		}
		ConfigCmd = 0;
		break;
	}
	return 0;
}

int	MAPINT	MapperSnd (int Len)
{
	return FDSsound::Get(Len);
}

unsigned char FDS_BIOS[2][0x1000];

BOOL	Load (void)
{
	FILE *BIOS;
	TCHAR buf[MAX_PATH];
	int i = GetModuleFileName(NULL, buf, MAX_PATH);
	if (!i)
	{
		MessageBox(hWnd, _T("Fatal error: failed to get directory!"), _T("FDS"), MSGBOX_FLAGS);
		return FALSE;
	}
	while (i > 0)
		if (buf[--i] == '\\')
			break;
	buf[i] = 0;
	if ((BIOS = _tfopen(_tcscat(buf, _T("\\disksys.rom")), _T("rb"))) == NULL)
	{
		MessageBox(hWnd, _T("Disk System BIOS (disksys.rom) not found!"), _T("FDS"), MSGBOX_FLAGS);
		return FALSE;
	}
	if ((fread(FDS_BIOS[0], 1, 0x1000, BIOS) != 0x1000) || (fread(FDS_BIOS[1], 1, 0x1000, BIOS) != 0x1000))
	{
		fclose(BIOS);
		MessageBox(hWnd, _T("Disk System BIOS (disksys.rom) too small!"), _T("FDS"), MSGBOX_FLAGS);
		return FALSE;
	}
	fclose(BIOS);
	EMU->StatusOut(_T("FDS BIOS loaded!"));
	ConfigWindow = NULL;
	FDSsound::Load();
	return TRUE;
}

void	Reset (RESET_TYPE ResetType)
{
	_Read = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, Read);
	_Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write);

	EMU->SetPRG_RAM32(0x6, 0);
	EMU->SetCHR_RAM8(0x0, 0);

	EMU->SetPRG_Ptr4(0xE, FDS_BIOS[0], FALSE);
	EMU->SetPRG_Ptr4(0xF, FDS_BIOS[1], FALSE);

	EndIRQ(0xFF);

	if (ResetType == RESET_HARD)
	{
		DiskNum = 0xFF;
		if (ConfigWindow)
		{
			EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_DISKSEL), TRUE);
			EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_INSERT), TRUE);
			EnableWindow(GetDlgItem(ConfigWindow, IDC_FDS_EJECT), FALSE);
		}
	}
	ConfigCmd = 0;

	FDSsound::Reset(ResetType);
}

void	Unload (void)
{
	FDSsound::Unload();
	if (ConfigWindow)
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}
} // namespace FDS
