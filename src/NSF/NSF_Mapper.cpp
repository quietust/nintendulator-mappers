/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_NSF.h"
#include	"..\Hardware\Sound\s_FDS.h"
#include	"..\Hardware\Sound\s_SUN5.h"
#include	"..\Hardware\Sound\s_MMC5.h"
#include	"..\Hardware\Sound\s_VRC6.h"
#include	"..\Hardware\Sound\s_VRC7.h"
#include	"..\Hardware\Sound\s_N163.h"
#include	"resource.h"
#include	<commctrl.h>
#include	<stdio.h>
#include	<stdlib.h>

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

.org $3F00
.db $FF,$FF,$FF			;pad to 256 bytes
reset:		SEI
		LDX #$FF
		STX $4017	;kill frame IRQs
		INX
		JSR silence	;silence all sound channels
		STX $2000	;we don't rely on the presence of a PPU
		STX $2001	;but we don't want it getting in the way
		STX $3E12	;kill PLAY timer
		CLI
loop:		JMP loop

irq:		PHA
		TXA
		PHA
		TYA
		PHA
		LDX $3E12	;check IRQ status
		BEQ init
		DEX
		BEQ reset	;"Proper" way to play a tune
		JSR song_play	;1) Call the play address of the music at periodic intervals determined by the speed words.
		PLA
		TAY
		PLA
		TAX
		PLA
		RTI

.module	silence
silence:			;X=0 coming in, must also be coming out
		STX $4015	;silence all sound channels
		LDA $3E13
		LSR A
		BCC _1
		STX $9002
		STX $A002
		STX $B002	;stop VRC6
_1:		LSR A
		BCC _2
		LDY #$20
__2:		STY $9010
		STX $9030	;stop VRC7
		INY
		CPY #$26
		BNE __2
_2:		LSR A
		BCC _3
		LDY #$80
		STY $4083
		STY $4087
		STY $4089	;stop FDS
_3:		LSR A
		BCC _4
		STX $5015	;stop MMC5
_4:		LSR A
		BCC _5
		DEX
		STX $F800
		INX
		STX $4800	;stop Namco-163
_5:		LSR A
		BCC _6
		LDY #$07
		STY $C000
		STY $E000	;stop SUN5
_6:		RTS

.module	init			;"Proper" way to init a tune
init:		JSR silence
		TXA		;(X=0)
		DEX
		TXS		;clear the stack
		STX $5FF7	;1.5) Map RAM to $6000-$7FFF
		DEX
		STX $5FF6	;(banks FE/FF are treated as RAM)

		LDX #$7F
		STA $00
		STX $01
		TAY
		LDX #$27
_1:		STA ($00),Y	;1) Clear all RAM at $0000-$07FF.
		INY		;2) Clear all RAM at $6000-$7FFF.
		BNE _1
		DEX
		BMI _2
		DEC $01
		CPX #$07
		BNE _1
		STX $01
		BEQ _1

_2:		LDX #$14
_3:		DEX
		STA $4000,X	;3) Init the sound registers by writing $00 to $4000-$4013
		BNE _3

		LDX #$07
_4:		LDA $3E08,X	;5) If this is a banked tune, load the bank values from the header into $5FF8-$5FFF.
		STA $5FF8,X	;For this player, *all* tunes are considered banked (the loader will fill in appropriate values)
		DEX
		BPL _4

		LDY #$0F	;4) Set volume register $4015 to $0F.
		STY $4015

		LDA $3E13	;For FDS games, banks E/F also get mapped to 6/7
		AND #$04	;For non-FDS games, we don't want to do this
		BEQ _6
		LDA $3E0E	;if these are zero, let's leave them as plain RAM
		BEQ _5
		STA $5FF6
_5:		LDA $3E0F	;TODO - need to allow FDS NSF data to be writeable
		BEQ _6
		STA $5FF7

_6:		LDX $3E11	;4.5) Set up the PLAY timer. Which word to use is determined by which mode you are in - PAL or NTSC.
		LDA $3E04,X	;X is 0 for NTSC and 1 for PAL, as needed for #6 below
		STA $3E10
		LDA $3E06,X
		STA $3E11
		STY $3E12
		LDA $3E12	;if we have a pending IRQ here, we need to cancel it NOW
		CLI		;re-enable interrupts now - this way, we can allow the init routine to not return (ex: for raw PCM)

		LDA $3E10	;6) Set the accumulator and X registers for the desired song.
		JSR song_init	;7) Call the music init routine.
		STA $3E13	;kill the watchdog timer (and fire a 'Play' interrupt after 1 frame; otherwise, it'll wait 4 frames)
		JMP loop	;we don't actually RTI from here; this way, the init code is faster

.module	song
song_init:	JMP ($3E00)
song_play:	JMP ($3E02)
.dw reset,irq	;no NMI vector
.end
*/

#define	NSFIRQ_NONE	0xFF
#define	NSFIRQ_INIT	0x00
#define	NSFIRQ_STOP	0x01
#define	NSFIRQ_PLAY	0x02

#define	NSFSOUND_VRC6	0x01
#define	NSFSOUND_VRC7	0x02
#define	NSFSOUND_FDS	0x04
#define	NSFSOUND_MMC5	0x08
#define	NSFSOUND_N163	0x10
#define	NSFSOUND_SUN5	0x20
#define	NSFSOUND_MASK	0x3F

namespace
{
uint8 ExRAM[1024];
FCPUWrite _Write4;
FCPURead _Read4, _ReadF;
HWND ControlWindow;

uint8 songnum;
uint8 ntscpal;
uint32 IRQcounter;
uint16_n IRQlatch;
uint8 IRQenabled;
uint8 IRQstatus;
uint8 WatchDog;

uint16_n InitAddr, PlayAddr, NTSCspeed, PALspeed;

unsigned char BIOS[256] =
{
	0xFF,0xFF,0xFF,0x78,0xA2,0xFF,0x8E,0x17,0x40,0xE8,0x20,0x30,0x3F,0x8E,0x00,0x20,
	0x8E,0x01,0x20,0x8E,0x12,0x3E,0x58,0x4C,0x17,0x3F,0x48,0x8A,0x48,0x98,0x48,0xAE,
	0x12,0x3E,0xF0,0x59,0xCA,0xF0,0xDC,0x20,0xF9,0x3F,0x68,0xA8,0x68,0xAA,0x68,0x40,
	0x8E,0x15,0x40,0xAD,0x13,0x3E,0x4A,0x90,0x09,0x8E,0x02,0x90,0x8E,0x02,0xA0,0x8E,
	0x02,0xB0,0x4A,0x90,0x0D,0xA0,0x20,0x8C,0x10,0x90,0x8E,0x30,0x90,0xC8,0xC0,0x26,
	0xD0,0xF5,0x4A,0x90,0x0B,0xA0,0x80,0x8C,0x83,0x40,0x8C,0x87,0x40,0x8C,0x89,0x40,
	0x4A,0x90,0x03,0x8E,0x15,0x50,0x4A,0x90,0x08,0xCA,0x8E,0x00,0xF8,0xE8,0x8E,0x00,
	0x48,0x4A,0x90,0x08,0xA0,0x07,0x8C,0x00,0xC0,0x8C,0x00,0xE0,0x60,0x20,0x30,0x3F,
	0x8A,0xCA,0x9A,0x8E,0xF7,0x5F,0xCA,0x8E,0xF6,0x5F,0xA2,0x7F,0x85,0x00,0x86,0x01,
	0xA8,0xA2,0x27,0x91,0x00,0xC8,0xD0,0xFB,0xCA,0x30,0x0A,0xC6,0x01,0xE0,0x07,0xD0,
	0xF2,0x86,0x01,0xF0,0xEE,0xA2,0x14,0xCA,0x9D,0x00,0x40,0xD0,0xFA,0xA2,0x07,0xBD,
	0x08,0x3E,0x9D,0xF8,0x5F,0xCA,0x10,0xF7,0xA0,0x0F,0x8C,0x15,0x40,0xAD,0x13,0x3E,
	0x29,0x04,0xF0,0x10,0xAD,0x0E,0x3E,0xF0,0x03,0x8D,0xF6,0x5F,0xAD,0x0F,0x3E,0xF0,
	0x03,0x8D,0xF7,0x5F,0xAE,0x11,0x3E,0xBD,0x04,0x3E,0x8D,0x10,0x3E,0xBD,0x06,0x3E,
	0x8D,0x11,0x3E,0x8C,0x12,0x3E,0xAD,0x12,0x3E,0x58,0xAD,0x10,0x3E,0x20,0xF6,0x3F,
	0x8D,0x13,0x3E,0x4C,0x17,0x3F,0x6C,0x00,0x3E,0x6C,0x02,0x3E,0x03,0x3F,0x1A,0x3F
};

int	MAPINT	ReadVector (int Bank, int Addr)
{
	if ((Addr >= 0xFFC) && (IRQstatus != NSFIRQ_NONE))
		return BIOS[Addr & 0xFF];
	else	return _ReadF(Bank, Addr);
}

void	IRQ (int type)
{
	IRQstatus = type;
	EMU->SetIRQ(type == NSFIRQ_NONE);
	if (type != NSFIRQ_NONE)
		EMU->SetCPUReadHandler(0xF, ReadVector);
	else	EMU->SetCPUReadHandler(0xF, _ReadF);
}

int	MAPINT	Read (int Bank, int Addr)
{
	if (Addr >= 0xF00)
		return BIOS[Addr & 0xFF];
	else if (Addr < 0xE00)
		return 0xFF;
	switch (Addr & 0xFF)
	{
	case 0x00:	return InitAddr.b0;			break;
	case 0x01:	return InitAddr.b1;			break;
	case 0x02:	return PlayAddr.b0;			break;
	case 0x03:	return PlayAddr.b1;			break;
	case 0x04:	return NTSCspeed.b0;		break;
	case 0x06:	return NTSCspeed.b1;		break;
	case 0x05:	return PALspeed.b0;			break;
	case 0x07:	return PALspeed.b1;			break;
	case 0x08:	case 0x09:	case 0x0A:	case 0x0B:
	case 0x0C:	case 0x0D:	case 0x0E:	case 0x0F:
			return ROM->NSF_InitBanks[Addr & 0x7];	break;
	case 0x10:	return songnum;			break;
	case 0x11:	return ntscpal;			break;
	case 0x12:	{
				uint8 result = IRQstatus;
				IRQ(NSFIRQ_NONE);
				return result;
			}					break;
	case 0x13:	return ROM->NSF_SoundChips & NSFSOUND_MASK;	break;
	default:	return 0xFF;
	}
}
void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xFF)
	{
	case 0x10:	IRQlatch.b0 = Val;	break;
	case 0x11:	IRQlatch.b1 = Val;	break;
	case 0x12:	IRQcounter = IRQlatch.s0 * 5;
			WatchDog = 1;
			IRQenabled = Val;	break;
	case 0x13:	IRQcounter = IRQlatch.s0;
			WatchDog = 0;	break;
	}
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		IRQcounter--;
		if (!IRQcounter)
		{
			if (WatchDog)
			{
				EMU->DbgOut(_T("Watchdog timer triggered - this NSF either uses RAW PCM or is corrupted!"));
				WatchDog = 0;
			}
			IRQcounter = IRQlatch.s0;
			IRQ(NSFIRQ_PLAY);
		}
	}
}

LRESULT CALLBACK ControlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR NSFTitle[32], NSFArtist[32], NSFCopyright[32];
	int i = songnum;
	switch (message)
	{
	case WM_INITDIALOG:
#ifdef UNICODE
		mbstowcs(NSFTitle, ROM->NSF_Title, 32);
		mbstowcs(NSFArtist, ROM->NSF_Artist, 32);
		mbstowcs(NSFCopyright, ROM->NSF_Copyright, 32);
#else
		strcpy(NSFTitle, ROM->NSF_Title);
		strcpy(NSFArtist, ROM->NSF_Artist);
		strcpy(NSFCopyright, ROM->NSF_Copyright);
#endif
		SetDlgItemText(hDlg, IDC_NSF_TITLE, NSFTitle);
		SetDlgItemText(hDlg, IDC_NSF_ARTIST, NSFArtist);
		SetDlgItemText(hDlg, IDC_NSF_COPYRIGHT, NSFCopyright);
		SetDlgItemInt(hDlg, IDC_NSF_SONGS, ROM->NSF_NumSongs, FALSE);
		SetDlgItemInt(hDlg, IDC_NSF_PLAYING, songnum + 1, FALSE);
		if (ROM->NSF_NTSCPAL == 2)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_NSF_NTSC), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_NSF_PAL), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_NSF_NTSC), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_NSF_PAL), FALSE);
		}
		CheckRadioButton(hDlg, IDC_NSF_NTSC, IDC_NSF_PAL, (ntscpal ? IDC_NSF_PAL : IDC_NSF_NTSC));
		SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_SETRANGEMIN, TRUE, 0);
		SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_SETRANGEMAX, TRUE, ROM->NSF_NumSongs - 1);
		SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_SETPOS, TRUE, songnum);
		SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_SETPAGESIZE, 0, 1);
		SetDlgItemInt(hDlg, IDC_NSF_SELECTED, songnum + 1, FALSE);

		{
			TCHAR chiplist[9];
			uint8 c = ROM->NSF_SoundChips;
			_stprintf(chiplist, _T("%i%i%i%i%i%i%i%i"),
				(c & 0x80) ? 1 : 0,
				(c & 0x40) ? 1 : 0,
				(c & NSFSOUND_SUN5) ? 1 : 0,
				(c & NSFSOUND_N163) ? 1 : 0,
				(c & NSFSOUND_MMC5) ? 1 : 0,
				(c & NSFSOUND_FDS) ? 1 : 0,
				(c & NSFSOUND_VRC7) ? 1 : 0,
				(c & NSFSOUND_VRC6) ? 1 : 0);
			SetDlgItemText(hDlg, IDC_NSF_CHIPLIST, chiplist);
		}

		if (ROM->NSF_SoundChips == NSFSOUND_VRC6)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Konami VRC6"));
		else if (ROM->NSF_SoundChips == NSFSOUND_VRC7)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Konami VRC7"));
		else if (ROM->NSF_SoundChips == NSFSOUND_FDS)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Famicom Disk System"));
		else if (ROM->NSF_SoundChips == NSFSOUND_MMC5)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Nintendo MMC5"));
		else if (ROM->NSF_SoundChips == NSFSOUND_N163)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Namco 163"));
		else if (ROM->NSF_SoundChips == NSFSOUND_SUN5)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Sunsoft 5"));
		else if (ROM->NSF_SoundChips == 0x40)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Unknown"));
		else if (ROM->NSF_SoundChips == 0x80)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Unknown"));
		else if (ROM->NSF_SoundChips == 0)
			SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("No expansion sound"));
		else	SetDlgItemText(hDlg, IDC_NSF_CHIP, _T("Multiple"));

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_NSF_PREV:
			i -= 2;
		case IDC_NSF_NEXT:
			i++;
			SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_SETPOS, TRUE, i);
			SetDlgItemInt(hDlg, IDC_NSF_SELECTED, SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_GETPOS, 0, 0) + 1, FALSE);
		case IDC_NSF_PLAY:
			songnum = GetDlgItemInt(hDlg, IDC_NSF_SELECTED, NULL, FALSE) - 1;
			SetDlgItemInt(hDlg, IDC_NSF_PLAYING, songnum + 1, FALSE);
			if (IsDlgButtonChecked(hDlg, IDC_NSF_PAL) == BST_CHECKED)
				ntscpal = 1;
			else	ntscpal = 0;
			IRQ(NSFIRQ_INIT);
			break;
		case IDC_NSF_STOP:
			IRQ(NSFIRQ_STOP);
			break;
		case IDCLOSE:
			ControlWindow = NULL;
			DestroyWindow(hDlg);
			break;
		}
		break;
	case WM_HSCROLL:
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_NSF_SELECT))
			SetDlgItemInt(hDlg, IDC_NSF_SELECTED, SendDlgItemMessage(hDlg, IDC_NSF_SELECT, TBM_GETPOS, 0, 0) + 1, FALSE);
		break;
	case WM_CLOSE:
		ControlWindow = NULL;
		DestroyWindow(hDlg);
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
			if (ControlWindow)
				break;
			ControlWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_NSF), hWnd, (DLGPROC)ControlProc);
			SetWindowPos(ControlWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
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

int	MAPINT	MapperSnd (int Cycles)
{
	int i = 0;
	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
		i += VRC6sound::Get(Cycles);
	if (ROM->NSF_SoundChips & NSFSOUND_VRC7)
		i += VRC7sound::Get(Cycles);
	if (ROM->NSF_SoundChips & NSFSOUND_FDS)
		i += FDSsound::Get(Cycles);
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
		i += MMC5sound::Get(Cycles);
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		i += N163sound::Get(Cycles);
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
		i += SUN5sound::Get(Cycles);
	return i;
}

int	MAPINT	Read4 (int Bank, int Addr)
{
	if (Addr < 0x018)
		return _Read4(Bank, Addr);
	if ((ROM->NSF_SoundChips & NSFSOUND_FDS) && (Addr < 0x800))
		return FDSsound::Read((Bank << 12) | Addr);
	if ((ROM->NSF_SoundChips & NSFSOUND_N163) && (Addr & 0x800))
		return N163sound::Read((Bank << 12) | Addr);
	return -1;
}
int	MAPINT	Read5 (int Bank, int Addr)
{
	switch (Addr & 0xF00)
	{
	case 0x000:	return MMC5sound::Read((Bank << 12) | Addr);
	case 0xC00:
	case 0xD00:
	case 0xE00:
	case 0xF00:	return ExRAM[Addr & 0x3FF];	break;
	}
	return -1;
}
void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if (Addr < 0x018)
		_Write4(Bank, Addr, Val);
	if (ROM->NSF_SoundChips & NSFSOUND_FDS)
		FDSsound::Write((Bank << 12) | Addr, Val);
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		N163sound::Write((Bank << 12) | Addr, Val);
}
void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr >= 0xFF6)
	{
		if (Val >= 0xFE)
			EMU->SetPRG_RAM4(Addr & 0xF, Val & 0x1);
		else
		{
			EMU->SetPRG_ROM4(Addr & 0xF, Val);
			if (ROM->NSF_SoundChips & NSFSOUND_FDS)
				EMU->SetPRG_Ptr4(Addr & 0xF, EMU->GetPRG_Ptr4(Addr & 0xF), TRUE);
		}
	}
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
	{
		switch (Addr & 0xF00)
		{
		case 0x000:	MMC5sound::Write((Bank << 12) | Addr, Val);	break;
		case 0xC00:
		case 0xD00:
		case 0xE00:
		case 0xF00:	ExRAM[Addr & 0x3FF] = Val;		break;
		}
	}
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
		VRC6sound::Write((Bank << 12) | Addr, Val);
	if (ROM->NSF_SoundChips & NSFSOUND_VRC7)
		VRC7sound::Write((Bank << 12) | Addr, Val);
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	VRC6sound::Write((Bank << 12) | Addr, Val);
}
void	MAPINT	WriteCDE (int Bank, int Addr, int Val)
{
	SUN5sound::Write((Bank << 12) | Addr, Val);
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		N163sound::Write((Bank << 12) | Addr, Val);
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
		SUN5sound::Write((Bank << 12) | Addr, Val);
}

void	MAPINT	Load (void)
{
	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
		VRC6sound::Load();
	if (ROM->NSF_SoundChips & NSFSOUND_VRC7)
		VRC7sound::Load();
	if (ROM->NSF_SoundChips & NSFSOUND_FDS)
		FDSsound::Load();
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
		MMC5sound::Load();
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		N163sound::Load();
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
		SUN5sound::Load();
	ControlWindow = NULL;

	InitAddr.s0 = ROM->NSF_InitAddr;
	PlayAddr.s0 = ROM->NSF_PlayAddr;
	NTSCspeed.s0 = (int)(ROM->NSF_NTSCSpeed * (double)1.789772727272727);
	PALspeed.s0 = (int)(ROM->NSF_PALSpeed * (double)1.662607);
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Read4 = EMU->GetCPUReadHandler(0x4);
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	_ReadF = EMU->GetCPUReadHandler(0xF);

	EMU->SetCPUReadHandler(0x3, Read);
	EMU->SetCPUWriteHandler(0x3, Write);

	if (ROM->NSF_SoundChips & (NSFSOUND_N163 | NSFSOUND_FDS))
		EMU->SetCPUReadHandler(0x4, Read4);
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
		EMU->SetCPUReadHandler(0x5, Read5);

	if (ROM->NSF_SoundChips & (NSFSOUND_N163 | NSFSOUND_FDS))
		EMU->SetCPUWriteHandler(0x4, Write4);
	EMU->SetCPUWriteHandler(0x5, Write5);
	if (ROM->NSF_SoundChips & (NSFSOUND_VRC6 | NSFSOUND_VRC7))
		EMU->SetCPUWriteHandler(0x9, Write9);
	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
	{
		EMU->SetCPUWriteHandler(0xA, WriteAB);
		EMU->SetCPUWriteHandler(0xB, WriteAB);
	}
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
	{
		EMU->SetCPUWriteHandler(0xC, WriteCDE);
		EMU->SetCPUWriteHandler(0xD, WriteCDE);
		EMU->SetCPUWriteHandler(0xE, WriteCDE);
	}
	if (ROM->NSF_SoundChips & (NSFSOUND_N163 | NSFSOUND_SUN5))
		EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
		VRC6sound::Reset(ResetType);
	if (ROM->NSF_SoundChips & NSFSOUND_VRC7)
		VRC7sound::Reset(ResetType);
	if (ROM->NSF_SoundChips & NSFSOUND_FDS)
		FDSsound::Reset(ResetType);
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
		MMC5sound::Reset(ResetType);
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		N163sound::Reset(ResetType);
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
		SUN5sound::Reset(ResetType);
	if (ResetType == RESET_HARD)
	{
		songnum = ROM->NSF_InitSong - 1;
		if (ROM->NSF_NTSCPAL == 1)
			ntscpal = 1;
		else	ntscpal = 0;
	}
	IRQ(NSFIRQ_INIT);		// Initialize first tune and start playing it
					// (this also allows it to fetch the RESET vector)
	Config(CFG_WINDOW, TRUE);
}

void	MAPINT	Unload (void)
{
	if (ControlWindow)
		DestroyWindow(ControlWindow);

	if (ROM->NSF_SoundChips & NSFSOUND_VRC6)
		VRC6sound::Unload();
	if (ROM->NSF_SoundChips & NSFSOUND_VRC7)
		VRC7sound::Unload();
	if (ROM->NSF_SoundChips & NSFSOUND_FDS)
		FDSsound::Unload();
	if (ROM->NSF_SoundChips & NSFSOUND_MMC5)
		MMC5sound::Unload();
	if (ROM->NSF_SoundChips & NSFSOUND_N163)
		N163sound::Unload();
	if (ROM->NSF_SoundChips & NSFSOUND_SUN5)
		SUN5sound::Unload();
}
} // namespace
const MapperInfo MapperInfo_NSF =
{
	NULL,
	_T("NES Sound File"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	NULL,
	MapperSnd,
	Config
};