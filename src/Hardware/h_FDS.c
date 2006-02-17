#include	"h_FDS.h"
#include	"Sound\s_FDS.h"
#include	"..\FDS\resource.h"
#include	<stdio.h>

TFDS	FDS;

int	_MAPINT	FDS_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,FDS.DiskNum)
	SAVELOAD_WORD(mode,x,data,FDS.IRQcounter)
	SAVELOAD_WORD(mode,x,data,FDS.IRQlatch.s0)
	SAVELOAD_BYTE(mode,x,data,FDS.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,FDS.IOenable)
	SAVELOAD_BYTE(mode,x,data,FDS.IOcontrol)
	SAVELOAD_BYTE(mode,x,data,FDS.IOstatus)
	SAVELOAD_WORD(mode,x,data,FDS.BytePtr)
	SAVELOAD_BYTE(mode,x,data,FDS.WriteSkip)
	SAVELOAD_BYTE(mode,x,data,FDS.DiskIRQ)
	SAVELOAD_BYTE(mode,x,data,FDS.Mirror)
	x = FDSsound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
	{
		if (FDS.Mirror)
			EMU->Mirror_H();
		else	EMU->Mirror_V();
	}
	return x;
}

#define	IRQ_TIMER	0x01
#define	IRQ_DISK	0x02

static	__inline void	StartIRQ (u8 flag)
{
	FDS.IOstatus |= flag;
	EMU->SetIRQ(0);
}

static	__inline void	EndIRQ (u8 flag)
{
	FDS.IOstatus &= ~flag;
	if (!FDS.IOstatus)
		EMU->SetIRQ(1);
}

void	_MAPINT	FDS_CPUCycle (void)
{
	if ((FDS.IRQenabled & 0x2) && (FDS.IRQcounter) && (!--FDS.IRQcounter))
	{
		if (FDS.IRQenabled & 0x1)
			FDS.IRQcounter = FDS.IRQlatch.s0;
		else	FDS.IRQenabled &= 1;
		StartIRQ(IRQ_TIMER);
	}
	if ((FDS.DiskIRQ) && (!--FDS.DiskIRQ) && (FDS.IOcontrol & 0x80))
		StartIRQ(IRQ_DISK);
}

#define	DISKIRQ_SHORT	80
#define	DISKIRQ_LONG	255

int	_MAPINT	FDS_Read (int Bank, int Addr)
{
	u8 result;
	static u8 DiskVal = 0;
	if ((Addr & 0xFFF) < 0x18)
		return FDS.Read(Bank,Addr);
	switch (Addr & 0xFFF)
	{
	case 0x30:	result = FDS.IOstatus;
			EndIRQ(0xFF);	// everything
			return result;		break;
	case 0x31:	if (FDS.DiskNum == 0xFF)
				return DiskVal;
			EMU->SetPRG_ROM4(0x5,(FDS.DiskNum << 4) | ((FDS.BytePtr >> 12) & 0xF));
			DiskVal = EMU->GetPRG_Ptr4(0x5)[FDS.BytePtr & 0xFFF];
			EMU->SetPRG_OB4(0x5);
//			if (FDS.IOcontrol & 0x01)
			{
				if (FDS.BytePtr < 64999)
					FDS.BytePtr++;
				if ((FDS.BytePtr & 0xFF) == 0)
					EMU->StatusOut("%i R/S",FDS.BytePtr);
				FDS.DiskIRQ = DISKIRQ_SHORT;
				EndIRQ(IRQ_DISK);
			}
			return DiskVal;			break;
	case 0x32:	result = 0x40;
			if (FDS.DiskNum == 0xFF)
				result |= 0x05;
			if ((FDS.DiskNum == 0xFF) || (!(FDS.IOcontrol & 0x01)) || (FDS.IOcontrol & 0x02))
				result |= 0x02;
			return result;			break;
	case 0x33:	return 0x80;			break;
	}
	return FDSsound_Read((Bank << 12) | Addr);
}

void	_MAPINT	FDS_Write (int Bank, int Addr, int Val)
{
	if ((Addr & 0xFFF) < 0x18)
		FDS.Write(Bank,Addr,Val);
	FDSsound_Write((Bank << 12) | Addr,Val);
	switch (Addr & 0xFFF)
	{
	case 0x20:	FDS.IRQlatch.b0 = Val;
			EndIRQ(IRQ_TIMER);		break;
	case 0x21:	FDS.IRQlatch.b1 = Val;
			EndIRQ(IRQ_TIMER);		break;
	case 0x22:	FDS.IRQenabled = Val & 0x3;
			FDS.IRQcounter = FDS.IRQlatch.s0;
			EndIRQ(IRQ_TIMER);		break;
	case 0x23:	FDS.IOenable = Val;		break;
	case 0x24:	if ((FDS.DiskNum != 0xFF) && (!(FDS.IOcontrol & 0x04)) && (FDS.IOenable & 0x1))
			{
				if ((FDS.BytePtr >= 0) && (FDS.BytePtr < 65000))
				{
					if (FDS.WriteSkip) FDS.WriteSkip--;
					else if (FDS.BytePtr >= 2)
					{
						EMU->StatusOut("%i W",FDS.BytePtr);
						FDS.BytePtr -= 2;
						EMU->SetPRG_ROM4(0x5,(FDS.DiskNum << 4) | ((FDS.BytePtr >> 12) & 0xF));
						EMU->GetPRG_Ptr4(0x5)[FDS.BytePtr & 0xFFF] = Val;
						EMU->SetPRG_OB4(0x5);
						FDS.BytePtr += 2;
					}
				}
			}				break;
	case 0x25:	EndIRQ(IRQ_DISK);
			if (FDS.Mirror = Val & 0x8)
				EMU->Mirror_H();
			else	EMU->Mirror_V();
			if (FDS.DiskNum == 0xFF)	break;
			if (!(Val & 0x40))
			{
				if ((FDS.IOcontrol & 0x40) && (!(Val & 0x10)))
				{
					EMU->StatusOut("%i S (Reverse)",FDS.BytePtr);
					FDS.BytePtr -= 2;
					FDS.DiskIRQ = DISKIRQ_LONG;
				}
				if (FDS.BytePtr < 0)
					FDS.BytePtr = 0;
			}
			if (!(Val & 0x04))
				FDS.WriteSkip = 2;
			if (Val & 0x02)
			{
				EMU->StatusOut("%i S (Zero)",FDS.BytePtr);
				FDS.BytePtr = 0;
				FDS.DiskIRQ = DISKIRQ_LONG;
			}
			if (Val & 0x40)
				FDS.DiskIRQ = DISKIRQ_LONG;
			FDS.IOcontrol = Val;
			break;
	}
}

static	void	_MAPINT	WriteBIOS (int Bank, int Addr, int Val)
{	/* don't allow writing to BIOS! */	}


static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	char buf[256];
	switch (message)
	{
		case WM_INITDIALOG:
			for (i = 0; i < ROM->FDS_NumSides; i++)
			{
				sprintf(buf,"Disk %i Side %s",(i >> 1)+1,(i & 1) ? "B" : "A");
				SendDlgItemMessage(hDlg,IDC_FDS_DISKSEL,CB_ADDSTRING,0,(LPARAM)buf);
			}
			if (FDS.DiskNum == 0xFF)
			{
				SendDlgItemMessage(hDlg,IDC_FDS_DISKSEL,CB_SETCURSEL,0,0);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_DISKSEL),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_INSERT),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_EJECT),FALSE);
			}
			else
			{
				SendDlgItemMessage(hDlg,IDC_FDS_DISKSEL,CB_SETCURSEL,FDS.DiskNum,0);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_DISKSEL),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_INSERT),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_EJECT),TRUE);
			}
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_FDS_EJECT:
				FDS.ConfigCmd = 0xFF;
				return TRUE;		break;
			case IDC_FDS_INSERT:
				FDS.ConfigCmd = (u8)SendDlgItemMessage(hDlg,IDC_FDS_DISKSEL,CB_GETCURSEL,0,0) + 1;
				return TRUE;		break;
			case IDCLOSE:
				FDS.ConfigWindow = NULL;
				DestroyWindow(hDlg);
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			FDS.ConfigWindow = NULL;
			DestroyWindow(hDlg);
			return TRUE;		break;
	}
	return FALSE;
}

unsigned char	_MAPINT	FDS_Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (FDS.ConfigWindow)
				break;
			FDS.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_FDS),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(FDS.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return FDS.ConfigCmd;
		break;
	case CFG_CMD:
		if (data)
		{
			if (data == 0xFF)
			{
				FDS.DiskNum = 0xFF;
				EMU->StatusOut("Disk ejected!");
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_DISKSEL),TRUE);
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_INSERT),TRUE);
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_EJECT),FALSE);
			}
			else
			{
				FDS.DiskNum = data - 1;
				EMU->StatusOut("Disk %i side %s inserted!",(FDS.DiskNum >> 1) + 1, (FDS.DiskNum & 1) ? "B" : "A");
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_DISKSEL),FALSE);
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_INSERT),FALSE);
				EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_EJECT),TRUE);
			}
		}
		FDS.ConfigCmd = 0;
		break;
	}
	return 0;
}

int	_MAPINT	FDS_MapperSnd (int Len)
{
	return FDSsound_Get(Len);
}

unsigned char FDS_BIOS[2][0x1000];

void	FDS_Load (void)
{
	FILE *BIOS;
	char buf[256];
	int i;
	if (!(i = GetModuleFileName(NULL,buf,255)))
	{
		MessageBox(hWnd,"Fatal error: failed to get directory!","FDS.DLL",MSGBOX_FLAGS);
		return;
	}
	while (i > 0)
		if (buf[--i] == '\\')	break;
	buf[i] = 0;
	if ((BIOS = fopen(strcat(buf,"\\disksys.rom"),"rb")) == NULL)
	{
		MessageBox(hWnd,"Disk System BIOS (disksys.rom) not found!","FDS.DLL",MSGBOX_FLAGS);
		return;
	}
	if ((fread(FDS_BIOS[0],1,0x1000,BIOS) != 0x1000) || (fread(FDS_BIOS[1],1,0x1000,BIOS) != 0x1000))
	{
		fclose(BIOS);
		MessageBox(hWnd,"Disk System BIOS (disksys.rom) too small!","FDS.DLL",MSGBOX_FLAGS);
		return;
	}
	fclose(BIOS);
	EMU->StatusOut("FDS BIOS loaded!");
	FDS.ConfigWindow = NULL;
	FDSsound_Load();
}

void	FDS_Reset (RESET_TYPE ResetType)
{
	FDS.Read = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,FDS_Read);
	FDS.Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,FDS_Write);

	EMU->SetPRG_RAM32(0x6,0);
	EMU->SetCHR_RAM8(0,0);

	EMU->SetPRG_Ptr4(0xE,FDS_BIOS[0],FALSE);
	EMU->SetPRG_Ptr4(0xF,FDS_BIOS[1],FALSE);

	EndIRQ(-1);

	if (ResetType == RESET_HARD)
	{
		FDS.DiskNum = 0xFF;
		if (FDS.ConfigWindow)
		{
			EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_DISKSEL),TRUE);
			EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_INSERT),TRUE);
			EnableWindow(GetDlgItem(FDS.ConfigWindow,IDC_FDS_EJECT),FALSE);
		}
	}
	FDS.ConfigCmd = 0;

	FDSsound_Reset(ResetType);
}

void	FDS_Unload (void)
{
	FDSsound_Unload();
	if (FDS.ConfigWindow)
		DestroyWindow(FDS.ConfigWindow);
}