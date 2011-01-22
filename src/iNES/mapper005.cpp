/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC5.h"
#include	"resource.h"

namespace
{
HWND ConfigWindow;

unsigned long CRCtable[256];
int	initialized = 0;
void	InitCRC (void)
{
	const unsigned long poly = 0xEDB88320;
	unsigned long n;
	if (initialized)
		return;
	for (int i = 0; i < 256; i++)
	{
		n = i;
		for (int j = 0; j < 8; j++)
		{
			if (n & 1)
				n = (n >> 1) ^ poly;
			else	n >>= 1;
		}
		CRCtable[i] = n;
	}
	initialized = 1;
}
unsigned long	CalcCRC (unsigned char *data, int len)
{
	unsigned long CRC = 0xFFFFFFFF;
	InitCRC();
	for (int i = 0; i < len; i++)
		CRC = (CRC >> 8) ^ CRCtable[(CRC ^ data[i]) & 0xFF];
	return CRC ^ 0xFFFFFFFF;
}
struct
{
	unsigned long CRC;
	int SRAMconf;
} MMC5Carts[23] = {
	// ELROM
	{0x95CA9EC7,MMC5WRAM_0KB_0KB},	// Castlevania III - Dracula's Curse (U)
	{0x51D2112F,MMC5WRAM_0KB_0KB},	// Laser Invasion (U)
	{0x255B129C,MMC5WRAM_0KB_0KB},	// Gun Sight (J)
	{0xCD9ACF43,MMC5WRAM_0KB_0KB},	// Metal Slader Glory (J)
	{0xD979C8B7,MMC5WRAM_0KB_0KB},	// Uchuu Keibitai SDF (J)

	// EKROM
	{0xE7C72DBB,MMC5WRAM_8KB_0KB},	// Gemfire (U)
	{0x57F33F70,MMC5WRAM_8KB_0KB},	// *Royal Blood (J) (other site claims ETROM, but this is the same game as Gemfire which is EKROM)
	{0x5D9D9891,MMC5WRAM_8KB_0KB},	// Just Breed (J)
	{0xE91548D8,MMC5WRAM_8KB_0KB},	// Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J)

	// ETROM
	{0x2B548D75,MMC5WRAM_8KB_8KB},	// Bandit Kings of Ancient China (U)
	{0xE6C28C5F,MMC5WRAM_8KB_8KB},	// Suikoden - Tenmei no Chikai (J)
	{0x2F50BD38,MMC5WRAM_8KB_8KB},	// L'Empereur (U)
	{0x8E9A5E2F,MMC5WRAM_8KB_8KB},	// L'Empereur (U) [a1]
	{0x57E3218B,MMC5WRAM_8KB_8KB},	// L'Empereur (J)
	{0xB56958D1,MMC5WRAM_8KB_8KB},	// Nobunaga's Ambition 2 (U)
	{0x98C8E090,MMC5WRAM_8KB_8KB},	// Nobunaga no Yabou - Sengoku Gunyuu Den (J)
	{0xCD35E2E9,MMC5WRAM_8KB_8KB},	// Uncharted Waters (U)
	{0xF4CD4998,MMC5WRAM_8KB_8KB},	// Daikoukai Jidai (J)
	{0x8FA95456,MMC5WRAM_8KB_8KB},	// Ishin no Arashi (J)

	// EWROM
	{0x11EAAD26,MMC5WRAM_32KB_0KB},	// Romance of the Three Kingdoms II (U)
	{0x286613D8,MMC5WRAM_32KB_0KB},	// Nobunaga no Yabou - Bushou Fuuun Roku (J)
	{0x95BA5733,MMC5WRAM_32KB_0KB},	// Sangokushi 2 (J)
	{0xF4120E58,MMC5WRAM_32KB_0KB},	// *Aoki Ookami to Shiroki Mejika - Genchou Hishi (J) (other site claims ETROM, but it explicitly maps banks 0/1/2/3)
};

int	CheckSRAM (void)
{
	int orig, size = MMC5WRAM_MAXOPTS;
	unsigned long CRC;
	// If the ROM is NES 2.0, look at the PRG RAM size byte
	if (ROM->INES_Version == 2)
	{
		switch (ROM->INES2_PRGRAM)
		{
		// officially supported boards
		case 0x00:	return MMC5WRAM_0KB_0KB;	break;	// ELROM
		case 0x70:	return MMC5WRAM_8KB_0KB;	break;	// EKROM
		case 0x77:	return MMC5WRAM_8KB_8KB;	break;	// ETROM
		case 0x90:	return MMC5WRAM_32KB_0KB;	break;	// EWROM
		// unofficial combinations, but actually supported by MMC5
		case 0x07:	return MMC5WRAM_0KB_8KB;	break;
		case 0x09:	return MMC5WRAM_0KB_32KB;	break;
		case 0x79:	return MMC5WRAM_8KB_32KB;	break;
		case 0x97:	return MMC5WRAM_32KB_8KB;	break;
		case 0x99:	return MMC5WRAM_32KB_32KB;	break;
		// none of the above
		default:
			EMU->DbgOut(_T("NES 2.0 SRAM sizes specified are not supported by the MMC5! Attempting ROM CRC32 lookup..."));
			break;
		}
	}

	orig = EMU->GetPRG_ROM4(8);

	EMU->SetPRG_ROM4(8, 0);
	CRC = CalcCRC(EMU->GetPRG_Ptr4(8), ROM->INES_PRGSize << 14);
	for (int i = 0; i < 23; i++)
		if (MMC5Carts[i].CRC == CRC)
			size = MMC5Carts[i].SRAMconf;

	if (orig == -1)
		EMU->SetPRG_OB4(8);
	else	EMU->SetPRG_ROM4(8, orig);
	return size;
}

void	SetSRAM (void);
INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		switch (MMC5::GetRAMSize())
		{
		default:
		case MMC5WRAM_0KB_0KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_0_0);	break;
		case MMC5WRAM_0KB_8KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_0_8);	break;
		case MMC5WRAM_0KB_32KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_0_32);	break;
		case MMC5WRAM_8KB_0KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_8_0);	break;
		case MMC5WRAM_8KB_8KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_8_8);	break;
		case MMC5WRAM_8KB_32KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_8_32);	break;
		case MMC5WRAM_32KB_0KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_32_0);	break;
		case MMC5WRAM_32KB_8KB:	CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_32_8);	break;
		case MMC5WRAM_32KB_32KB:CheckRadioButton(hDlg, IDC_MAPPER5_0_0, IDC_MAPPER5_32_32, IDC_MAPPER5_32_32);	break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_0_0) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_0KB_0KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_0_8) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_0KB_8KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_0_32) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_0KB_32KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_8_0) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_8KB_0KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_8_8) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_8KB_8KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_8_32) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_8KB_32KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_32_0) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_32KB_0KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_32_8) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_32KB_8KB);
			else if (IsDlgButtonChecked(hDlg, IDC_MAPPER5_32_32) == BST_CHECKED)
				MMC5::SetRAMSize(MMC5WRAM_32KB_32KB);
			SetSRAM();
			MMC5::SyncPRG();	
			MessageBox(hWnd, _T("Please perform a SOFT reset to ensure proper functionality!"), _T("INES.DLL"), MB_OK);
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
			ConfigWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAPPER5), hWnd, ConfigProc);
			SetWindowPos(ConfigWindow, hWnd, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return FALSE;
		break;
	case CFG_QUERY:
		break;
	case CFG_CMD:
		break;
	}
	return 0;
}

void	SetSRAM (void)
{
	switch (MMC5::GetRAMSize())
	{
	case MMC5WRAM_0KB_0KB:
	case MMC5WRAM_0KB_8KB:
	case MMC5WRAM_0KB_32KB:
		break;

	case MMC5WRAM_8KB_0KB:
	case MMC5WRAM_8KB_8KB:
	case MMC5WRAM_8KB_32KB:
		if (ROM->INES_Flags & 0x02)
			EMU->Set_SRAMSize(8192);
		break;

	case MMC5WRAM_32KB_0KB:
	case MMC5WRAM_32KB_8KB:
	case MMC5WRAM_32KB_32KB:
		if (ROM->INES_Flags & 0x02)
			EMU->Set_SRAMSize(32768);
		break;

	case MMC5WRAM_MAXOPTS:
		MMC5::SetRAMSize(MMC5WRAM_32KB_8KB);
		if (ROM->INES_Flags & 0x02)
			EMU->Set_SRAMSize(32768);
		EMU->DbgOut(_T("Unable to determine SRAM size for this game, falling back to 32KB/8KB! Please set it via the GAME menu!"));
		break;
	}
}

void	MAPINT	Load (void)
{
	MMC5::Load(CheckSRAM());
	SetSRAM();
	ConfigWindow = NULL;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC5::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC5::Unload();
	if (ConfigWindow)
	{
		DestroyWindow(ConfigWindow);
		ConfigWindow = NULL;
	}
}

uint8 MapperNum = 5;
} // namespace

const MapperInfo MapperInfo_005 =
{
	&MapperNum,
	_T("MMC5"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC5::PPUCycle,
	MMC5::SaveLoad,
	MMC5::MapperSnd,
	Config
};
