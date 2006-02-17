#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC5.h"
#include	"resource.h"

static	struct
{
	HWND ConfigWindow;
}	Mapper;

static	unsigned long CRCtable[256];
static	int	initialized = 0;
static	void	InitCRC (void)
{
	const unsigned long poly = 0xEDB88320;
	unsigned long n;
	int i, j;
	if (initialized)
		return;
	for (i = 0; i < 256; i++)
	{
		n = i;
		for (j = 0; j < 8; j++)
		{
			if (n & 1)
				n = (n >> 1) ^ poly;
			else	n >>= 1;
		}
		CRCtable[i] = n;
	}
	initialized = 1;
}
static	unsigned long	CalcCRC (unsigned char *data, int len)
{
	unsigned long CRC = 0xFFFFFFFF;
	int i;
	InitCRC();
	for (i = 0; i < len; i++)
		CRC = (CRC >> 8) ^ CRCtable[(CRC ^ data[i]) & 0xFF];
	return CRC ^ 0xFFFFFFFF;
}
static	struct
{
	unsigned long CRC;
	int SRAMconf;
} MMC5Carts[23] = {
	{0x95CA9EC7,MMC5WRAM_0KB_0KB},	// Castlevania III - Dracula's Curse (U)
	{0x51D2112F,MMC5WRAM_0KB_0KB},	// Laser Invasion (U)
	{0x255B129C,MMC5WRAM_0KB_0KB},	// (?) Gun Sight (J)

	{0xE7C72DBB,MMC5WRAM_8KB_0KB},	// Gemfire (U)
	{0x57F33F70,MMC5WRAM_8KB_0KB},	// (?) Royal Blood (J)
	{0x5D9D9891,MMC5WRAM_8KB_0KB},	// (?) Just Breed (J)

	{0x2B548D75,MMC5WRAM_8KB_8KB},	// Bandit Kings of Ancient China (U)
	{0xE6C28C5F,MMC5WRAM_8KB_8KB},	// (?) Suikoden - Tenmei no Chikai (J)
	{0x2F50BD38,MMC5WRAM_8KB_8KB},	// L'Empereur (U)
	{0x8E9A5E2F,MMC5WRAM_8KB_8KB},	// L'Empereur (U) [a1]
	{0x57E3218B,MMC5WRAM_8KB_8KB},	// (?) L'Empereur (J)
	{0xB56958D1,MMC5WRAM_8KB_8KB},	// Nobunaga's Ambition 2 (U)
	{0x98C8E090,MMC5WRAM_8KB_8KB},	// (?) Nobunaga no Yabou - Sengoku Gunyuu Den (J)
	{0xCD35E2E9,MMC5WRAM_8KB_8KB},	// Uncharted Waters (U)
	{0xF4CD4998,MMC5WRAM_8KB_8KB},	// (?) Daikoukai Jidai (J)

	{0x11EAAD26,MMC5WRAM_32KB_0KB},	// Romance of the Three Kingdoms II (U)
	{0x95BA5733,MMC5WRAM_32KB_0KB},	// (?) Sangokushi 2 (J)

	{0xF4120E58,MMC5WRAM_MAXOPTS},	// Aoki Ookami to Shiroki Mejika - Genchou Hishi (J)
	{0x8FA95456,MMC5WRAM_MAXOPTS},	// Ishin no Arashi (J)
	{0xCD9ACF43,MMC5WRAM_MAXOPTS},	// Metal Slader Glory (J)
	{0x286613D8,MMC5WRAM_MAXOPTS},	// Nobunaga no Yabou - Bushou Fuuun Roku (J)
	{0xE91548D8,MMC5WRAM_MAXOPTS},	// Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J)
	{0xD979C8B7,MMC5WRAM_MAXOPTS}	// Uchuu Keibitai SDF (J)
};

static	int	CheckSRAM (void)
{
	int orig, i, size = MMC5WRAM_MAXOPTS;
	unsigned long CRC;
	orig = EMU->GetPRG_ROM4(8);

	EMU->SetPRG_ROM4(8,0);
	CRC = CalcCRC(EMU->GetPRG_Ptr4(8),ROM->INES_PRGSize << 14);
	for (i = 0; i < 23; i++)
		if (MMC5Carts[i].CRC == CRC)
			size = MMC5Carts[i].SRAMconf;

	if (orig == -1)
		EMU->SetPRG_OB4(8);
	else	EMU->SetPRG_ROM4(8,orig);
	return size;
}

static	void	SetSRAM (void);
static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			switch (MMC5.WRAMsize)
			{
			case MMC5WRAM_0KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_0);	break;
			case MMC5WRAM_0KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_8);	break;
			case MMC5WRAM_0KB_32KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_32);	break;
			case MMC5WRAM_8KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_0);	break;
			case MMC5WRAM_8KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_8);	break;
			case MMC5WRAM_8KB_32KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_8_32);	break;
			case MMC5WRAM_32KB_0KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_0);	break;
			case MMC5WRAM_32KB_8KB:	CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_8);	break;
			case MMC5WRAM_32KB_32KB:CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_32_32);	break;
			default:		CheckRadioButton(hDlg,IDC_MAPPER5_0_0,IDC_MAPPER5_32_32,IDC_MAPPER5_0_0);	break;
			}
			SetDlgItemText(hDlg,IDC_MAPPER5_DESCRIBE,
				"The following Famicom games have unknown SRAM sizes:\r\n"
				"Aoki Ookami to Shiroki Mejika - Genchou Hishi\r\n"
				"Ishin no Arashi\r\n"
				"Metal Slader Glory\r\n"
				"Nobunaga no Yabou - Bushou Fuuun Roku\r\n"
				"Shin 4 Nin Uchi Mahjong - Yakuman Tengoku\r\n"
				"Uchuu Keibitai SDF\r\n"
				"If you have any of these games, please email the author with the game PCB's board name (HVC-E*ROM)."
				);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_0) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_0KB_0KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_8) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_0KB_8KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_0_32) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_0KB_32KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_0) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_8KB_0KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_8) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_8KB_8KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_8_32) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_8KB_32KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_0) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_32KB_0KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_8) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_32KB_8KB;
				else if (IsDlgButtonChecked(hDlg,IDC_MAPPER5_32_32) == BST_CHECKED)
					MMC5.WRAMsize = MMC5WRAM_32KB_32KB;
				SetSRAM();
				MMC5_SyncPRG();	
				MessageBox(hWnd,"Please perform a SOFT reset to ensure proper functionality!","INES.DLL",MB_OK);
			case IDCANCEL:
				Mapper.ConfigWindow = NULL;
				DestroyWindow(hDlg);
				return TRUE;		break;
			}
			break;
		case WM_CLOSE:
			Mapper.ConfigWindow = NULL;
			DestroyWindow(hDlg);
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
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAPPER5),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
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

static	void	SetSRAM (void)
{
	switch (MMC5.WRAMsize)
	{
	case MMC5WRAM_0KB_0KB:
	case MMC5WRAM_0KB_8KB:
	case MMC5WRAM_0KB_32KB:
		break;

	case MMC5WRAM_8KB_0KB:
	case MMC5WRAM_8KB_8KB:
	case MMC5WRAM_8KB_32KB:
		EMU->Set_SRAMSize(8192);
		break;

	case MMC5WRAM_32KB_0KB:
	case MMC5WRAM_32KB_8KB:
	case MMC5WRAM_32KB_32KB:
		EMU->Set_SRAMSize(32768);
		break;

	case MMC5WRAM_MAXOPTS:
		MMC5.WRAMsize = MMC5WRAM_32KB_8KB;
		EMU->Set_SRAMSize(32768);
		EMU->DbgOut("Unable to determine SRAM size for this game! Please set it via the GAME menu!");
		//Config();
		break;
	}
	if (ROM->INES_Flags & 0x02)
		EMU->Load_SRAM();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC5_Destroy();
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
	Mapper.ConfigWindow = NULL;
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	if (ResetType == RESET_HARD)
		MMC5.WRAMsize = CheckSRAM();

	Mapper.ConfigWindow = NULL;
	SetSRAM();

	MMC5_Init(ResetType);
}

static	u8 MapperNum = 5;
CTMapperInfo	MapperInfo_005 =
{
	&MapperNum,
	"MMC5",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	MMC5_PPUCycle,
	MMC5_SaveLoad,
	MMC5_MapperSnd,
	Config
};