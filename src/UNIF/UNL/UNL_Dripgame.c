#include	"..\..\DLL\d_UNIF.h"
#include	"..\resource.h"

static	struct
{
	struct	MSChan
	{
		u8 FIFO[256], ReadPos, WritePos;
		BOOL IsFull, IsEmpty;
		int freq, vol;
		int timer;
		int Pos;
	} Chan[2];
}	MapSound;
static	int	MapperSnd_GenerateWave (struct MSChan *Chan, int Cycles)
{
	int x = 0, y;
	for (y = 0; y < Cycles; y++)
	{
		if (Chan->IsEmpty)
			break;
		if (!Chan->timer--)
		{
			Chan->timer = Chan->freq;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsFull = FALSE;
			Chan->Pos = (Chan->FIFO[++Chan->ReadPos] - 0x80) * Chan->vol;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsEmpty = TRUE;
		}
		x += Chan->Pos;
	}
	return x / Cycles;
}
static	int	MAPINT	MapperSnd (int Cycles)
{
	int out = 0;
	out += MapperSnd_GenerateWave(&MapSound.Chan[0],Cycles);
	out += MapperSnd_GenerateWave(&MapSound.Chan[1],Cycles);
	return out << 3;
}
static	int	MAPINT	MapperSnd_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	int i;
	for (i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].FIFO[i])
	for (i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].FIFO[i])
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].ReadPos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].ReadPos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].WritePos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].WritePos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].IsFull)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].IsFull)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].IsEmpty)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].IsEmpty)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[0].freq)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[1].freq)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].vol)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].vol)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[0].timer)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[1].timer)
	return x;
}

static	struct
{
	u8 PRG, CHR[4], Flags;
	u8 IRQenabled, IRQlatch;
	u16 IRQcounter;
	u8 *ExtRam0, *ExtRam1;
	u16 LastAddr, LastAddrTmp;
	u8 Jumper;
	FPPURead PPUReadNT[4];
	FCPURead Read4;
	HWND ConfigWindow;
	u8 ConfigCmd;
}	Mapper;

int	MAPINT	ReadNT (int Bank, int Addr)
{
	if ((Mapper.Flags & 0x4) && (Addr >= 0x3C0) && ((Bank & 3) == ((Mapper.LastAddr >> 10) & 3)))
	{
		const unsigned char AttribBits[4] = {0x00,0x55,0xAA,0xFF};
		if (EMU->GetCHR_NT1(Bank))
			return AttribBits[Mapper.ExtRam1[Mapper.LastAddr & 0x3FF]];
		else	return AttribBits[Mapper.ExtRam0[Mapper.LastAddr & 0x3FF]];
	}
	else	return Mapper.PPUReadNT[Bank & 3](Bank,Addr);
}

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,0xF);
	EMU->SetCHR_ROM2(0x0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(0x2,Mapper.CHR[1]);
	EMU->SetCHR_ROM2(0x4,Mapper.CHR[2]);
	EMU->SetCHR_ROM2(0x6,Mapper.CHR[3]);
	switch (Mapper.Flags & 0x3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (Mapper.Flags & 0x04)
	{
		EMU->SetPPUReadHandler(0x8,ReadNT);
		EMU->SetPPUReadHandler(0x9,ReadNT);
		EMU->SetPPUReadHandler(0xA,ReadNT);
		EMU->SetPPUReadHandler(0xB,ReadNT);
	}
	else
	{
		EMU->SetPPUReadHandler(0x8,Mapper.PPUReadNT[0]);
		EMU->SetPPUReadHandler(0x9,Mapper.PPUReadNT[1]);
		EMU->SetPPUReadHandler(0xA,Mapper.PPUReadNT[2]);
		EMU->SetPPUReadHandler(0xB,Mapper.PPUReadNT[3]);
	}
	if (Mapper.Flags & 0x8)
		EMU->SetPRG_RAM8(0x6,0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	x = MapperSnd_SaveLoad(mode,x,data);
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Flags)
	SAVELOAD_WORD(mode,x,data,Mapper.LastAddr)
	SAVELOAD_WORD(mode,x,data,Mapper.LastAddrTmp)
	SAVELOAD_BYTE(mode,x,data,Mapper.Jumper)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled) && (!--Mapper.IRQcounter))
		EMU->SetIRQ(0);
}
static	void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	Mapper.LastAddr = Mapper.LastAddrTmp;
	Mapper.LastAddrTmp = Addr;
}


static	int	MAPINT	Read4 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return Mapper.Jumper | 'd';
	else	return Mapper.Read4(Bank,Addr);
}
static	int	MAPINT	Read5 (int Bank, int Addr)
{
	struct MSChan *Chan;
	int result = 0;
	if (Addr & 0x800)
		Chan = &MapSound.Chan[1];
	else	Chan = &MapSound.Chan[0];
	if (Chan->IsFull)
		result |= 0x80;
	if (Chan->IsEmpty)
		result |= 0x40;
	return result;
}
static	void	MAPINT	WriteL (int Bank, int Addr, int Val)
{
	if (Addr & 0x8)
	{
		switch (Addr & 0x7)
		{
		case 0:	Mapper.IRQlatch = Val;
			break;
		case 1:	Mapper.IRQcounter = ((Val & 0x7F) << 8) | Mapper.IRQlatch;
			Mapper.IRQenabled = Val & 0x80;
			EMU->SetIRQ(1);
			break;
		case 2:	Mapper.Flags = Val & 0xF;
			break;
		case 3:	Mapper.PRG = Val & 0xF;
			break;
		case 4:	Mapper.CHR[0] = Val & 0xF;
			break;
		case 5:	Mapper.CHR[1] = Val & 0xF;
			break;
		case 6:	Mapper.CHR[2] = Val & 0xF;
			break;
		case 7:	Mapper.CHR[3] = Val & 0xF;
			break;
		}
		Sync();
	}
	else
	{
		struct MSChan *Chan;
		if (Addr & 4)
			Chan = &MapSound.Chan[1];
		else	Chan = &MapSound.Chan[0];

		switch (Addr & 3)
		{
		case 0x0:
			memset(Chan->FIFO,0,256);
			Chan->ReadPos = Chan->WritePos = 0;
			Chan->IsFull = FALSE;
			Chan->IsEmpty = TRUE;
			Chan->Pos = 0;
			Chan->timer = Chan->freq;
			break;
		case 0x1:
			if (Chan->ReadPos == Chan->WritePos)
			{
				Chan->IsEmpty = FALSE;
				Chan->Pos = Val * Chan->vol;
			}
			Chan->FIFO[Chan->WritePos++] = Val;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsFull = TRUE;
			break;
		case 0x2:
			Chan->freq = (Chan->freq & 0xF00) | Val;
			break;
		case 0x3:
			Chan->freq = (Chan->freq & 0xFF) | ((Val & 0xF) << 8);
			Chan->vol = (Val & 0xF0) >> 4;
			Chan->Pos = Chan->FIFO[Chan->ReadPos] * Chan->vol;
			break;
		}
	}
}
static	void	MAPINT	WriteH (int Bank, int Addr, int Val)
{
	if (Addr & 0x400)
		Mapper.ExtRam1[Addr & 0x3FF] = Val & 3;
	else	Mapper.ExtRam0[Addr & 0x3FF] = Val & 3;
}

static	LRESULT CALLBACK ConfigProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg,IDC_UNL_DRIPGAME_J0,(Mapper.Jumper & 0x80) ? BST_CHECKED : BST_UNCHECKED);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				Mapper.ConfigCmd = 0x80;
				if (IsDlgButtonChecked(hDlg,IDC_UNL_DRIPGAME_J0) == BST_CHECKED)
					Mapper.ConfigCmd |= 0x01;
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

static	unsigned char	MAPINT	Config (CFG_TYPE mode, unsigned char data)
{
	switch (mode)
	{
	case CFG_WINDOW:
		if (data)
		{
			if (Mapper.ConfigWindow)
				break;
			Mapper.ConfigWindow = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_UNL_DRIPGAME),hWnd,(DLGPROC)ConfigProc);
			SetWindowPos(Mapper.ConfigWindow,hWnd,0,0,0,0,SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else	return TRUE;
		break;
	case CFG_QUERY:
		return Mapper.ConfigCmd;
		break;
	case CFG_CMD:
		if (data & 0x80)
		{
			Mapper.Jumper = (data & 0x01) ? 0x80 : 0x00;
			Sync();
		}
		Mapper.ConfigCmd = 0;
		break;
	}
	return 0;
}

static	void	MAPINT	Load (void)
{
	Mapper.ConfigWindow = NULL;
	EMU->Mirror_4();
	Mapper.ExtRam0 = EMU->GetCHR_Ptr1(0xA);
	Mapper.ExtRam1 = EMU->GetCHR_Ptr1(0xB);
	UNIF_SetSRAM(8192);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	Mapper.Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,Read4);
	EMU->SetCPUReadHandler(0x5,Read5);
	for (x = 0x8; x < 0xC; x++)
		EMU->SetCPUWriteHandler(x,WriteL);
	for (x = 0xC; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,WriteH);

	Mapper.PPUReadNT[0] = EMU->GetPPUReadHandler(0x8);
	Mapper.PPUReadNT[1] = EMU->GetPPUReadHandler(0x9);
	Mapper.PPUReadNT[2] = EMU->GetPPUReadHandler(0xA);
	Mapper.PPUReadNT[3] = EMU->GetPPUReadHandler(0xB);
	
	memset(&MapSound,0,sizeof(MapSound));
	MapSound.Chan[0].IsEmpty = TRUE;
	MapSound.Chan[1].IsEmpty = TRUE;

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = 0;
		for (x = 0; x < 4; x++)
			Mapper.CHR[x] = x;
		Mapper.Flags = 0;
		Mapper.IRQenabled = Mapper.IRQlatch = 0;
		Mapper.IRQcounter = 0;
		Mapper.LastAddr = Mapper.LastAddrTmp = 0;
		Mapper.Jumper = 0;
		EMU->SetIRQ(1);
	}

	Sync();
}
static	void	MAPINT	Unload (void)
{
	if (Mapper.ConfigWindow)
		DestroyWindow(Mapper.ConfigWindow);
}

CTMapperInfo	MapperInfo_UNL_DRIPGAME =
{
	"UNL-DRIPGAME",
	_T("Drip"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	MapperSnd,
	Config
};
