#include	"..\DLL\d_NSF.h"
#include	"..\Hardware\Sound\s_FDS.h"
#include	"..\Hardware\Sound\s_FME7.h"
#include	"..\Hardware\Sound\s_MMC5.h"
#include	"..\Hardware\Sound\s_VRC6.h"
#include	"..\Hardware\Sound\s_VRC7.h"
#include	"..\Hardware\Sound\s_N106.h"
#include	"resource.h"
#include	<commctrl.h>
#include	<stdio.h>

/*
;NSF Mapper
;R $3E00-$3E01 - INIT address
;R $3E02-$3E03 - PLAY address
;R $3E04/$3E06 - NTSC speed value
;R $3E05/$3E07 - PAL speed value
;R $3E08-$3E0F - Initial bankswitch values
;R $3E10 - Song Number (start at 0)
;R $3E11 - NTSC/PAL setting (0 for NTSC, 1 for PAL)
;W $3E10-$3E11 - IRQ counter (for PLAY)
;R $3E12 - IRQ status (0=INIT, 1=STOP, 2+=PLAY)
;W $3E12 - IRQ enable (for PLAY)
;R $3E13 - Sound chip info
;W $3E13 - clear watchdog timer
*/


#define	NSFIRQ_NONE	0xFF
#define	NSFIRQ_INIT	0x00
#define	NSFIRQ_STOP	0x01
#define	NSFIRQ_PLAY	0x02

static	struct
{
	u8 ExRAM[1024];
	FCPUWrite Write4;
	FCPURead Read4, ReadF;
	HWND ControlWindow;

	u8 songnum;
	u8 ntscpal;
	u32 IRQcounter;
	u16_n IRQlatch;
	u8 IRQenabled;
	u8 IRQstatus;
	u8 WatchDog;
}	NSF;

static	unsigned char NSFROM[256] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0xD8,0xA2,0xFF,0x9A,0x8E,
	0x17,0x40,0xE8,0x8E,0x15,0x40,0x8E,0x00,0x20,0x8E,0x01,0x20,0x8E,0x12,0x3E,0x58,
	0x4C,0x20,0x3F,0xAE,0x12,0x3E,0xF0,0x56,0xCA,0xF0,0x04,0x20,0xF9,0x3F,0x40,0x8E,
	0x12,0x3E,0x8E,0x15,0x40,0xAD,0x13,0x3E,0x4A,0x90,0x09,0x8E,0x02,0x90,0x8E,0x02,
	0xA0,0x8E,0x02,0xB0,0x4A,0x90,0x0D,0xA0,0x20,0x8C,0x10,0x90,0x8E,0x30,0x90,0xC8,
	0xC0,0x26,0xD0,0xF5,0x4A,0x90,0x0B,0xA0,0x80,0x8C,0x83,0x40,0x8C,0x87,0x40,0x8C,
	0x89,0x40,0x4A,0x90,0x03,0x8E,0x15,0x50,0x4A,0x90,0x07,0x88,0x8E,0x00,0xF8,0x8E,
	0x00,0x48,0x4A,0x90,0x08,0xA0,0x07,0x8C,0x00,0xC0,0x8C,0x00,0xE0,0x40,0xA2,0xFF,
	0x9A,0x8E,0xF7,0x5F,0xCA,0x8E,0xF6,0x5F,0xA9,0x00,0xA2,0x7F,0x85,0x00,0x86,0x01,
	0xA8,0xA2,0x27,0x91,0x00,0xC8,0xD0,0xFB,0xCA,0x30,0x0A,0xC6,0x01,0xE0,0x07,0xD0,
	0xF2,0x86,0x01,0xF0,0xEE,0xA2,0x14,0xCA,0x9D,0x00,0x40,0xD0,0xFA,0xA2,0x07,0xBD,
	0x08,0x3E,0x9D,0xF8,0x5F,0xCA,0x10,0xF7,0xA0,0x0F,0x8C,0x15,0x40,0xAD,0x13,0x3E,
	0x29,0x04,0xF0,0x10,0xAD,0x0E,0x3E,0xF0,0x03,0x8D,0xF6,0x5F,0xAD,0x0F,0x3E,0xF0,
	0x03,0x8D,0xF7,0x5F,0xAE,0x11,0x3E,0xBD,0x04,0x3E,0x8D,0x10,0x3E,0xBD,0x06,0x3E,
	0x8D,0x11,0x3E,0x8C,0x12,0x3E,0xAD,0x12,0x3E,0x58,0xAD,0x10,0x3E,0x20,0xF6,0x3F,
	0x8D,0x13,0x3E,0x4C,0x20,0x3F,0x6C,0x00,0x3E,0x6C,0x02,0x3E,0x0A,0x3F,0x23,0x3F
};

static	void	NSF_IRQ (int type)
{
	NSF.IRQstatus = type;
	EMU->SetIRQ(type == NSFIRQ_NONE);
}

static	int	_MAPINT	NSF_Read (int Bank, int Addr)
{
	if (Addr >= 0xF00)
		return NSFROM[Addr & 0xFF];
	else if (Addr < 0xE00)
		return 0xFF;
	switch (Addr & 0xFF)
	{
	case 0x00:	return (ROM->NSF_InitAddr >> 0) & 0xFF;	break;
	case 0x01:	return (ROM->NSF_InitAddr >> 8) & 0xFF;	break;
	case 0x02:	return (ROM->NSF_PlayAddr >> 0) & 0xFF;	break;
	case 0x03:	return (ROM->NSF_PlayAddr >> 8) & 0xFF;	break;
	case 0x04:	return ((int)(ROM->NSF_NTSCSpeed * (double)1.789772727272727) >> 0) & 0xFF;	break;
	case 0x06:	return ((int)(ROM->NSF_NTSCSpeed * (double)1.789772727272727) >> 8) & 0xFF;	break;
	case 0x05:	return ((int)(ROM->NSF_PALSpeed * (double)1.662607) >> 0) & 0xFF;		break;
	case 0x07:	return ((int)(ROM->NSF_PALSpeed * (double)1.662607) >> 8) & 0xFF;		break;
	case 0x08:	case 0x09:	case 0x0A:	case 0x0B:
	case 0x0C:	case 0x0D:	case 0x0E:	case 0x0F:
			return ROM->NSF_InitBanks[Addr & 0x7];	break;
	case 0x10:	return NSF.songnum;			break;
	case 0x11:	return NSF.ntscpal;			break;
	case 0x12:	{
				u8 result = NSF.IRQstatus;
				NSF_IRQ(NSFIRQ_NONE);
				return result;
			}					break;
	case 0x13:	return ROM->NSF_SoundChips & 0x3F;	break;	// if more sound chips get added, I'll handle them later
	default:	return 0xFF;
	}
}
static	void	_MAPINT	NSF_Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xFF)
	{
	case 0x10:	NSF.IRQlatch.b0 = Val;	break;
	case 0x11:	NSF.IRQlatch.b1 = Val;	break;
	case 0x12:	NSF.IRQcounter = NSF.IRQlatch.s0 * 5;
			NSF.WatchDog = 1;
			NSF.IRQenabled = Val;	break;
	case 0x13:	NSF.IRQcounter = NSF.IRQlatch.s0;
			NSF.WatchDog = 0;	break;
	}
}

static	int	_MAPINT	NSF_ReadVector (int Bank, int Addr)
{
	if ((Addr >= 0xFFC) && (NSF.IRQstatus != NSFIRQ_NONE))
		return NSFROM[Addr & 0xFF];
	else	return NSF.ReadF(Bank,Addr);
}

static	void	_MAPINT	CPUCycle (void)
{
	if (NSF.IRQenabled)
	{
		NSF.IRQcounter--;
		if (!NSF.IRQcounter)
		{
			if (NSF.WatchDog)
				MessageBox(hWnd,"Warning: this NSF either uses RAW PCM or is corrupted!","NSF",MB_OK);
			NSF.IRQcounter = NSF.IRQlatch.s0;
			NSF_IRQ(NSFIRQ_PLAY);
		}
	}
}

static	LRESULT CALLBACK ControlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i = NSF.songnum;
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_NSF_TITLE,ROM->NSF_Title);
		SetDlgItemText(hDlg,IDC_NSF_ARTIST,ROM->NSF_Artist);
		SetDlgItemText(hDlg,IDC_NSF_COPYRIGHT,ROM->NSF_Copyright);
		SetDlgItemInt(hDlg,IDC_NSF_SONGS,ROM->NSF_NumSongs,FALSE);
		SetDlgItemInt(hDlg,IDC_NSF_PLAYING,NSF.songnum,FALSE);
		if (ROM->NSF_NTSCPAL == 2)
		{
			EnableWindow(GetDlgItem(hDlg,IDC_NSF_NTSC),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_NSF_PAL),TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg,IDC_NSF_NTSC),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_NSF_PAL),FALSE);
		}
		CheckRadioButton(hDlg,IDC_NSF_NTSC,IDC_NSF_PAL,(NSF.ntscpal ? IDC_NSF_PAL : IDC_NSF_NTSC));
		SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_SETRANGEMIN,TRUE,0);
		SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_SETRANGEMAX,TRUE,ROM->NSF_NumSongs - 1);
		SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_SETPOS,TRUE,NSF.songnum);
		SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_SETPAGESIZE,0,1);
		SetDlgItemInt(hDlg,IDC_NSF_SELECTED,NSF.songnum,FALSE);

		{
			char chiplist[9];
			u8 c = ROM->NSF_SoundChips;
			sprintf(chiplist,"%s%s%s%s%s%s%s%s",
				(c & 0x80) ? "1" : "0",
				(c & 0x40) ? "1" : "0",
				(c & 0x20) ? "1" : "0",
				(c & 0x10) ? "1" : "0",
				(c & 0x08) ? "1" : "0",
				(c & 0x04) ? "1" : "0",
				(c & 0x02) ? "1" : "0",
				(c & 0x01) ? "1" : "0");
			SetDlgItemText(hDlg,IDC_NSF_CHIPLIST,chiplist);
		}

		if (ROM->NSF_SoundChips & 0x01)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Konami VRC6");
		else if (ROM->NSF_SoundChips & 0x02)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Konami VRC7");
		else if (ROM->NSF_SoundChips & 0x04)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Famicom Disk System");
		else if (ROM->NSF_SoundChips & 0x08)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Nintendo MMC5");
		else if (ROM->NSF_SoundChips & 0x10)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Namco 106");
		else if (ROM->NSF_SoundChips & 0x20)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Sunsoft FME-07");
		else if (ROM->NSF_SoundChips & 0x40)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Unknown!");
		else if (ROM->NSF_SoundChips & 0x80)
			SetDlgItemText(hDlg,IDC_NSF_CHIP,"Unknown!");
		else	SetDlgItemText(hDlg,IDC_NSF_CHIP,"No expansion sound");

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_NSF_PREV:
			i -= 2;
		case IDC_NSF_NEXT:
			i++;
			SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_SETPOS,TRUE,i);
			SetDlgItemInt(hDlg,IDC_NSF_SELECTED,SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_GETPOS,0,0),FALSE);
		case IDC_NSF_PLAY:
			NSF.songnum = GetDlgItemInt(hDlg,IDC_NSF_SELECTED,NULL,FALSE);
			SetDlgItemInt(hDlg,IDC_NSF_PLAYING,NSF.songnum,FALSE);
			if (IsDlgButtonChecked(hDlg,IDC_NSF_PAL) == BST_CHECKED)
				NSF.ntscpal = 1;
			else	NSF.ntscpal = 0;
			NSF_IRQ(NSFIRQ_INIT);
			break;
		case IDC_NSF_STOP:
			NSF_IRQ(NSFIRQ_STOP);
			break;
		case IDCLOSE:
			NSF.ControlWindow = NULL;
			DestroyWindow(hDlg);
			break;
		}
		break;
	case WM_HSCROLL:
		if ((HWND)lParam == GetDlgItem(hDlg,IDC_NSF_SELECT))
			SetDlgItemInt(hDlg,IDC_NSF_SELECTED,SendDlgItemMessage(hDlg,IDC_NSF_SELECT,TBM_GETPOS,0,0),FALSE);
		break;
	case WM_CLOSE:
		NSF.ControlWindow = NULL;
		DestroyWindow(hDlg);
		break;
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
			if (NSF.ControlWindow)
				break;
			NSF.ControlWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_NSF),hWnd,(DLGPROC)ControlProc);
			SetWindowPos(NSF.ControlWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		break;
	case CFG_CMD:
		/* TODO - add signals for selecting NTSC/PAL so we can have the emulator switch with it */
		break;
	}
	return 0;
}

static	int	_MAPINT	MapperSnd (int Cycles)
{
	int x = 0;
	if (ROM->NSF_SoundChips & 0x01)
		x += VRC6sound_Get(Cycles);
	if (ROM->NSF_SoundChips & 0x02)
		x += VRC7sound_Get(Cycles);
	if (ROM->NSF_SoundChips & 0x04)
		x += FDSsound_Get(Cycles);
	if (ROM->NSF_SoundChips & 0x08)
		x += MMC5sound_Get(Cycles);
	if (ROM->NSF_SoundChips & 0x10)
		x += N106sound_Get(Cycles);
	if (ROM->NSF_SoundChips & 0x20)
		x += FME7sound_Get(Cycles);
	return x;
}

static	int	_MAPINT	NSF_Read4 (int Bank, int Addr)
{
	if (Addr < 0x018)
		return NSF.Read4(Bank,Addr);
	if ((ROM->NSF_SoundChips & 0x04) && (Addr < 0x800))
		return FDSsound_Read((Bank << 12) | Addr);
	if ((ROM->NSF_SoundChips & 0x10) && (Addr & 0x800))
		return N106sound_Read((Bank << 12) | Addr);
	return -1;
}
static	int	_MAPINT	NSF_Read5 (int Bank, int Addr)
{
	if (ROM->NSF_SoundChips & 0x08)
	{
		switch (Addr & 0xF00)
		{
		case 0xC00:
		case 0xD00:
		case 0xE00:
		case 0xF00:	return NSF.ExRAM[Addr & 0x3FF];	break;
		}
	}
	return -1;
}
static	void	_MAPINT	NSF_Write4 (int Bank, int Addr, int Val)
{
	if (Addr < 0x018)
		NSF.Write4(Bank,Addr,Val);
	if (ROM->NSF_SoundChips & 0x04)
		FDSsound_Write((Bank << 12) | Addr,Val);
	if (ROM->NSF_SoundChips & 0x10)
		N106sound_Write((Bank << 12) | Addr,Val);
}
static	void	_MAPINT	NSF_Write5 (int Bank, int Addr, int Val)
{
	if (Addr >= 0xFF6)
	{
		if (Val >= 0xFE)
			EMU->SetPRG_RAM4(Addr & 0xF,Val & 0x1);
		else	EMU->SetPRG_ROM4(Addr & 0xF,Val);
	}
	if (ROM->NSF_SoundChips & 0x08)
	{
		switch (Addr & 0xF00)
		{
		case 0x000:	MMC5sound_Write((Bank << 12) | Addr, Val);	break;
		case 0xC00:
		case 0xD00:
		case 0xE00:
		case 0xF00:	NSF.ExRAM[Addr & 0x3FF] = Val;		break;
		}
	}
}
static	void	_MAPINT	NSF_Write9 (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & 0x01)
		VRC6sound_Write((Bank << 12) | Addr,Val);
	if (ROM->NSF_SoundChips & 0x02)
		VRC7sound_Write((Bank << 12) | Addr,Val);
}
static	void	_MAPINT	NSF_WriteAB (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & 0x01)
		VRC6sound_Write((Bank << 12) | Addr,Val);
}
static	void	_MAPINT	NSF_WriteCDE (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & 0x20)
		FME7sound_Write((Bank << 12) | Addr,Val);
}
static	void	_MAPINT	NSF_WriteF (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & 0x10)
		N106sound_Write((Bank << 12) | Addr,Val);
	if (ROM->NSF_SoundChips & 0x20)
		FME7sound_Write((Bank << 12) | Addr,Val);
}

static	void	_MAPINT	Shutdown (void)
{
	if (NSF.ControlWindow)
		DestroyWindow(NSF.ControlWindow);
	NSF.ControlWindow = NULL;

	if (ROM->NSF_SoundChips & 0x01)
		VRC6sound_Destroy();
	if (ROM->NSF_SoundChips & 0x02)
		VRC7sound_Destroy();
	if (ROM->NSF_SoundChips & 0x04)
		FDSsound_Destroy();
	if (ROM->NSF_SoundChips & 0x08)
		MMC5sound_Destroy();
	if (ROM->NSF_SoundChips & 0x10)
		N106sound_Destroy();
	if (ROM->NSF_SoundChips & 0x20)
		FME7sound_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	NSF.Read4 = EMU->GetCPUReadHandler(0x4);
	NSF.Write4 = EMU->GetCPUWriteHandler(0x4);
	NSF.ReadF = EMU->GetCPUReadHandler(0xF);

	EMU->SetCPUReadHandler(0x3,NSF_Read);
	EMU->SetCPUWriteHandler(0x3,NSF_Write);

	EMU->SetCPUReadHandler(0xF,NSF_ReadVector);
	EMU->SetCHR_RAM8(0,0);
	
	EMU->SetCPUReadHandler(0x4,NSF_Read4);
	EMU->SetCPUReadHandler(0x5,NSF_Read5);
	EMU->SetCPUWriteHandler(0x4,NSF_Write4);
	EMU->SetCPUWriteHandler(0x5,NSF_Write5);
	EMU->SetCPUWriteHandler(0x9,NSF_Write9);
	EMU->SetCPUWriteHandler(0xA,NSF_WriteAB);
	EMU->SetCPUWriteHandler(0xB,NSF_WriteAB);
	EMU->SetCPUWriteHandler(0xC,NSF_WriteCDE);
	EMU->SetCPUWriteHandler(0xD,NSF_WriteCDE);
	EMU->SetCPUWriteHandler(0xE,NSF_WriteCDE);
	EMU->SetCPUWriteHandler(0xF,NSF_WriteF);

	if (ROM->NSF_SoundChips & 0x01)
		VRC6sound_Init();
	if (ROM->NSF_SoundChips & 0x02)
		VRC7sound_Init();
	if (ROM->NSF_SoundChips & 0x04)
		FDSsound_Init();
	if (ROM->NSF_SoundChips & 0x08)
		MMC5sound_Init();
	if (ROM->NSF_SoundChips & 0x10)
		N106sound_Init();
	if (ROM->NSF_SoundChips & 0x20)
		FME7sound_Init();
	if (ResetType == RESET_HARD)
	{
		NSF.songnum = ROM->NSF_InitSong - 1;
		if (ROM->NSF_NTSCPAL == 1)
			NSF.ntscpal = 1;
		else	NSF.ntscpal = 0;
	}
	NSF_IRQ(NSFIRQ_INIT);		// Initialize first tune and start playing it
					// (this also allows it to fetch the RESET vector)
	NSF.ControlWindow = NULL;
	Config(CFG_WINDOW,TRUE);
}

CTMapperInfo	MapperInfo_NSF =
{
	NULL,
	"NES Sound File",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	NULL,
	MapperSnd,
	Config
};