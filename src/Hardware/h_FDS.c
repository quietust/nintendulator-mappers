#include	"h_FDS.h"
#include	"Sound\s_FDS.h"
#include	"..\FDS\resource.h"
#include	<stdio.h>

TFDS	FDS;

int	_MAPINT	FDS_SaveLoad (int mode, int x, char *data)
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
	x = FDSsound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
	{
		EMU->SetPRG_RAM32(0x6,0);
		EMU->SetPRG_RAM4(0xE,8);
		EMU->SetPRG_RAM4(0xF,9);
		EMU->SetCHR_RAM8(0,0);
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

#define	DISKIRQ_SHORT	150
#define	DISKIRQ_LONG	200

int	_MAPINT	FDS_Read (int Bank, int Where)
{
	u8 result;
	static u8 DiskVal = 0;
	if ((Where & 0xFFF) < 0x18)
		return FDS.Read(Bank,Where);
	switch (Where & 0xFFF)
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
	return FDSsound_Read((Bank << 12) | Where);
}

void	_MAPINT	FDS_Write (int Bank, int Where, int What)
{
	if ((Where & 0xFFF) < 0x18)
		FDS.Write(Bank,Where,What);
	FDSsound_Write((Bank << 12) | Where,What);
	switch (Where & 0xFFF)
	{
	case 0x20:	FDS.IRQlatch.b0 = What;
			EndIRQ(IRQ_TIMER);		break;
	case 0x21:	FDS.IRQlatch.b1 = What;
			EndIRQ(IRQ_TIMER);		break;
	case 0x22:	FDS.IRQenabled = What & 0x3;
			FDS.IRQcounter = FDS.IRQlatch.s0;
			EndIRQ(IRQ_TIMER);		break;
	case 0x23:	FDS.IOenable = What;		break;
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
						EMU->GetPRG_Ptr4(0x5)[FDS.BytePtr & 0xFFF] = What;
						EMU->SetPRG_OB4(0x5);
						FDS.BytePtr += 2;
					}
				}
			}				break;
	case 0x25:	EndIRQ(IRQ_DISK);
			if (What & 0x08)
				EMU->Mirror_H();
			else	EMU->Mirror_V();
			if (FDS.DiskNum == 0xFF)	break;
			if (!(What & 0x40))
			{
				if ((FDS.IOcontrol & 0x40) && (!(What & 0x10)))
				{
					EMU->StatusOut("%i S (Reverse)",FDS.BytePtr);
					FDS.BytePtr -= 2;
					FDS.DiskIRQ = DISKIRQ_LONG;
				}
				if (FDS.BytePtr < 0)
					FDS.BytePtr = 0;
			}
			if (!(What & 0x04))
				FDS.WriteSkip = 2;
			if (What & 0x02)
			{
				EMU->StatusOut("%i S (Zero)",FDS.BytePtr);
				FDS.BytePtr = 0;
				FDS.DiskIRQ = DISKIRQ_LONG;
			}
			if (What & 0x40)
				FDS.DiskIRQ = DISKIRQ_LONG;
			FDS.IOcontrol = What;
			break;
	}
}

static	void	_MAPINT	WriteBIOS (int Bank, int Where, int What)
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
				FDS.DiskNum = 0xFF;
				EMU->StatusOut("Disk ejected!");
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_DISKSEL),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_INSERT),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_EJECT),FALSE);
				return TRUE;		break;
			case IDC_FDS_INSERT:
				FDS.DiskNum = (u8)SendDlgItemMessage(hDlg,IDC_FDS_DISKSEL,CB_GETCURSEL,0,0);
				EMU->StatusOut("Disk %i side %s inserted!",(FDS.DiskNum >> 1) + 1, (FDS.DiskNum & 1) ? "B" : "A");
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_DISKSEL),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_INSERT),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_FDS_EJECT),TRUE);
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

void	_MAPINT	FDS_Config (void)
{
	if (FDS.ConfigWindow)
		return;
	FDS.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_FDS),hWnd,(DLGPROC)ConfigProc);
	SetWindowPos(FDS.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
}

int	_MAPINT	FDS_MapperSnd (int Len)
{
	return FDSsound_Get(Len);
}

void	FDS_Init (int IsHardReset)
{
	FILE *BIOS;
	char buf[256];
	int i;

	FDS.Read = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,FDS_Read);
	FDS.Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,FDS_Write);

	EMU->SetPRG_RAM32(0x6,0);
	EMU->SetCHR_RAM8(0,0);

	EMU->SetPRG_RAM4(0xE,8);
	EMU->SetCPUWriteHandler(0xE,WriteBIOS);
	EMU->SetPRG_RAM4(0xF,9);
	EMU->SetCPUWriteHandler(0xF,WriteBIOS);


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
	if ((fread(EMU->GetPRG_Ptr4(0xE),1,4096,BIOS) != 4096) || (fread(EMU->GetPRG_Ptr4(0xF),1,4096,BIOS) != 4096))
	{
		fclose(BIOS);
		MessageBox(hWnd,"Disk System BIOS (disksys.rom) too small!","FDS.DLL",MSGBOX_FLAGS);
		return;
	}
	fclose(BIOS);

	EMU->StatusOut("FDS BIOS loaded!");

	EndIRQ(-1);

	if (IsHardReset)
		FDS.DiskNum = 0xFF;

	FDSsound_Init();
}

void	FDS_Destroy (void)
{
	FDSsound_Destroy();
	if (FDS.ConfigWindow)
	{
		DestroyWindow(FDS.ConfigWindow);
		FDS.ConfigWindow = NULL;
	}
}