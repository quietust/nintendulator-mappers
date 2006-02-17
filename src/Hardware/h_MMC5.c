#include	"h_MMC5.h"
#include	"Sound\s_MMC5.h"

TMMC5	MMC5;

//#define	MMC5_EXTENDED_VSPLIT	/* equivalent to selecting 'SL' mode on a cartridge rather than 'CL' mode */

static	const	unsigned char	AttribBits[4] = {0x00,0x55,0xAA,0xFF};
static	const	unsigned char	AttribLoc[256] =
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
	0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
};
static	const	unsigned char	AttribShift[128] =
{
	0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
	4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6
};

static	BOOL	InSplitArea = FALSE;
static	int	VScroll;
static	int	TileCache, CurTile;
static	const	int	WRAMtable[MMC5WRAM_MAXOPTS][8] = {
	{-1,-1,-1,-1,-1,-1,-1,-1},
	{ 0, 0, 0, 0,-1,-1,-1,-1},
	{ 0, 1, 2, 3,-1,-1,-1,-1},
	{-1,-1,-1,-1, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 1, 1, 1, 1},
	{ 0, 1, 2, 3, 4, 4, 4, 4},
	{-1,-1,-1,-1, 0, 1, 2, 3},
	{ 0, 0, 0, 0, 1, 2, 3, 4},
	{ 0, 1, 2, 3, 4, 5, 6, 7},
};

static	void	MMC5_SetPPUHandlers (void);

void	MMC5_Load (int WRAMsize)
{
	EMU->Mirror_4();
	MMC5.NameTable0		= EMU->GetCHR_Ptr1(0x8);
	MMC5.NameTable1		= EMU->GetCHR_Ptr1(0x9);
	MMC5.ExRAM		= EMU->GetCHR_Ptr1(0xA);
	MMC5.ExNameTable	= EMU->GetCHR_Ptr1(0xB);

	MMC5.WRAMsize = WRAMsize;
	MMC5sound_Load();
}

void	MMC5_Reset (RESET_TYPE ResetType)
{
	u8 x;

	MMC5.WritePPU = EMU->GetCPUWriteHandler(0x2);

	EMU->SetCPUWriteHandler(0x2,MMC5_WritePPU);
	EMU->SetCPUWriteHandler(0x3,MMC5_WritePPU);

	EMU->SetCPUReadHandler(0x5,MMC5_CPURead5);
	EMU->SetCPUWriteHandler(0x5,MMC5_CPUWrite5);
	MMC5.CPUWrite6F = EMU->GetCPUWriteHandler(0x8);
	for (x = 6; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,MMC5_CPUWrite6F);
	for (x = 0; x < 16; x++)
		MMC5.PPURead[x] = EMU->GetPPUReadHandler(x);
	
	MMC5.PRGsize = 3;
	MMC5.CHRsize = 0;
	
	MMC5.IRQenabled = MMC5.IRQreads = MMC5.IRQline = 0;
	MMC5.Mul1 = MMC5.Mul2 = MMC5.GfxMode = 0;
	
	MMC5.SplitMode = MMC5.SplitBank = MMC5.SplitScroll = 0;
	MMC5_SetPPUHandlers();

	for (x = 0; x < 8; x++)	MMC5.CHR_A[x].s0 = x;
	for (x = 0; x < 4; x++)	MMC5.CHR_B[x].s0 = x;
	for (x = 0; x < 2; x++)	MMC5.WRAMprot[x] = 0;
	MMC5.CHRhi = 0;

	for (x = 0; x < 5; x++)	MMC5.PRG[x] = 0xFF;

	TileCache = -1;
	MMC5sound_Reset(ResetType);
	MMC5_SyncPRG();
}

void	MMC5_Unload (void)
{
	MMC5sound_Unload();
}

int	_MAPINT	MMC5_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,MMC5.WRAMsize)
	SAVELOAD_BYTE(mode,x,data,MMC5.PRGsize)
	SAVELOAD_BYTE(mode,x,data,MMC5.CHRsize)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.WRAMprot[i])
	SAVELOAD_BYTE(mode,x,data,MMC5.GfxMode)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mirror)
	for (i = 0; i < 5; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_WORD(mode,x,data,MMC5.CHR_A[i].s0)
	for (i = 0; i < 4; i++)
		SAVELOAD_WORD(mode,x,data,MMC5.CHR_B[i].s0)
	SAVELOAD_BYTE(mode,x,data,MMC5.CHRhi)
	SAVELOAD_BYTE(mode,x,data,MMC5.CHRmode)
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitMode)
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitScroll)
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitBank)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQline)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQreads)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mul1)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mul2)
	SAVELOAD_WORD(mode,x,data,MMC5.LineCounter)
	SAVELOAD_BYTE(mode,x,data,MMC5.SpriteMode)
	x = MMC5sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
	{
		MMC5_SyncPRG();
		MMC5_SyncCHR(-1);
		MMC5_SyncMirror();
		MMC5_SetPPUHandlers();
	}
	return x;
}

void	MMC5_SetPRG (int Size, int Loc, int Bank)
{
	u8 x;
	if (Bank & 0x80)
	{
		if (Size == 8)		EMU->SetPRG_ROM8(Loc,Bank);
		else if (Size == 16)	EMU->SetPRG_ROM16(Loc,Bank >> 1);
		else if (Size == 32)	EMU->SetPRG_ROM32(Loc,Bank >> 2);
	}
	else
	{
		if (Size == 8)
		{
			if (WRAMtable[MMC5.WRAMsize][Bank & 0x7] == -1)
				for (x = Loc; x < Loc + 2; x++)
					EMU->SetPRG_OB4(x);
			else	EMU->SetPRG_RAM8(Loc,WRAMtable[MMC5.WRAMsize][Bank & 0x7]);
		}
		else if (Size == 16)
		{
			if (WRAMtable[MMC5.WRAMsize][Bank & 0x6] == -1)
				for (x = Loc; x < Loc + 4; x++)
					EMU->SetPRG_OB4(x);
			else
			{
				EMU->SetPRG_RAM8(Loc+0,WRAMtable[MMC5.WRAMsize][(Bank & 0x6) | 0]);
				EMU->SetPRG_RAM8(Loc+2,WRAMtable[MMC5.WRAMsize][(Bank & 0x6) | 1]);
			}
		}
	}
}

void	MMC5_SyncPRG (void)
{
		MMC5_SetPRG( 8,0x6,MMC5.PRG[0] & 0x7F);
	switch (MMC5.PRGsize)
	{
	case 0:	MMC5_SetPRG(32,0x8,MMC5.PRG[4] | 0x80);	break;
	case 1:	MMC5_SetPRG(16,0x8,MMC5.PRG[2]       );
		MMC5_SetPRG(16,0xC,MMC5.PRG[4] | 0x80);	break;
	case 2:	MMC5_SetPRG(16,0x8,MMC5.PRG[2]       );
		MMC5_SetPRG( 8,0xC,MMC5.PRG[3]       );
		MMC5_SetPRG( 8,0xE,MMC5.PRG[4] | 0x80);	break;
	case 3:	MMC5_SetPRG( 8,0x8,MMC5.PRG[1]       );
		MMC5_SetPRG( 8,0xA,MMC5.PRG[2]       );
		MMC5_SetPRG( 8,0xC,MMC5.PRG[3]	     );
		MMC5_SetPRG( 8,0xE,MMC5.PRG[4] | 0x80);	break;
	}
}

void	MMC5_SyncCHRA (void)
{
	switch (MMC5.CHRsize)
	{
	case 0:	EMU->SetCHR_ROM8(0,MMC5.CHR_A[7].s0);	break;
	case 1:	EMU->SetCHR_ROM4(0,MMC5.CHR_A[3].s0);
		EMU->SetCHR_ROM4(4,MMC5.CHR_A[7].s0);	break;
	case 2:	EMU->SetCHR_ROM2(0,MMC5.CHR_A[1].s0);
		EMU->SetCHR_ROM2(2,MMC5.CHR_A[3].s0);
		EMU->SetCHR_ROM2(4,MMC5.CHR_A[5].s0);
		EMU->SetCHR_ROM2(6,MMC5.CHR_A[7].s0);	break;
	case 3:	EMU->SetCHR_ROM1(0,MMC5.CHR_A[0].s0);
		EMU->SetCHR_ROM1(1,MMC5.CHR_A[1].s0);
		EMU->SetCHR_ROM1(2,MMC5.CHR_A[2].s0);
		EMU->SetCHR_ROM1(3,MMC5.CHR_A[3].s0);
		EMU->SetCHR_ROM1(4,MMC5.CHR_A[4].s0);
		EMU->SetCHR_ROM1(5,MMC5.CHR_A[5].s0);
		EMU->SetCHR_ROM1(6,MMC5.CHR_A[6].s0);
		EMU->SetCHR_ROM1(7,MMC5.CHR_A[7].s0);	break;
	}
}

void	MMC5_SyncCHRB (void)
{
	switch (MMC5.CHRsize)
	{
	case 0:	EMU->SetCHR_ROM8(0,MMC5.CHR_B[3].s0);	break;
	case 1:	EMU->SetCHR_ROM4(0,MMC5.CHR_B[3].s0);
		EMU->SetCHR_ROM4(4,MMC5.CHR_B[3].s0);	break;
	case 2:	EMU->SetCHR_ROM2(0,MMC5.CHR_B[1].s0);
		EMU->SetCHR_ROM2(2,MMC5.CHR_B[3].s0);
		EMU->SetCHR_ROM2(4,MMC5.CHR_B[1].s0);
		EMU->SetCHR_ROM2(6,MMC5.CHR_B[3].s0);	break;
	case 3:	EMU->SetCHR_ROM1(0,MMC5.CHR_B[0].s0);
		EMU->SetCHR_ROM1(1,MMC5.CHR_B[1].s0);
		EMU->SetCHR_ROM1(2,MMC5.CHR_B[2].s0);
		EMU->SetCHR_ROM1(3,MMC5.CHR_B[3].s0);
		EMU->SetCHR_ROM1(4,MMC5.CHR_B[0].s0);
		EMU->SetCHR_ROM1(5,MMC5.CHR_B[1].s0);
		EMU->SetCHR_ROM1(6,MMC5.CHR_B[2].s0);
		EMU->SetCHR_ROM1(7,MMC5.CHR_B[3].s0);	break;
	}
}

void	MMC5_SyncCHR (int mode)
{
	if ((TileCache != -1) || (InSplitArea))
		return;
	if ((MMC5.SpriteMode) && (MMC5.IRQreads & 0x01))
	{
		switch (mode)
		{
		case 0:	MMC5_SyncCHRA();	break;
		case 1:	MMC5_SyncCHRB();	break;
		}
	}
	else
	{
		if (MMC5.CHRmode)
			MMC5_SyncCHRB();
		else	MMC5_SyncCHRA();
	}
}

void	MMC5_SyncMirror (void)
{
	u8 mirror = MMC5.Mirror, i;
	for (i = 0; i < 4; i++)
	{
		if ((mirror & 3) == 3)
		{
			EMU->SetCHR_Ptr1(0x8|i,MMC5.ExNameTable,FALSE);
			EMU->SetCHR_Ptr1(0xC|i,MMC5.ExNameTable,FALSE);
		}
		else if (((mirror & 3) == 2) && (MMC5.GfxMode & 0x2))
		{
			EMU->SetCHR_NT1(0x8|i,1);
			EMU->SetCHR_NT1(0xC|i,1);
		}
		else
		{
			EMU->SetCHR_NT1(0x8|i,mirror & 3);
			EMU->SetCHR_NT1(0xC|i,mirror & 3);
		}
		mirror >>= 2;
	}
}

void	_MAPINT	MMC5_WritePPU (int Bank, int Addr, int Val)
{
	MMC5.WritePPU(Bank,Addr,Val);
	switch (Addr & 7)
	{
	case 0:	MMC5.SpriteMode = (Val & 0x20);	break;
	case 1:	if (!(Val & 0x18))
		{
			MMC5.LineCounter = -2;
			MMC5.IRQreads &= 0x80;
		}				break;
	}
}

int	_MAPINT	MMC5_CPURead5 (int Bank, int Addr)
{
	register u8 read = -1;
	switch (Addr & 0xF00)
	{
	case 0x000:
		read = MMC5sound_Read((Bank << 12) | Addr);		break;
	case 0x200:
		switch (Addr)
		{
		case 0x204:	read = MMC5.IRQreads & 0xC0;
				MMC5.IRQreads &= ~0x80;
				EMU->SetIRQ(1);					break;
		case 0x205:	read = ((MMC5.Mul1 * MMC5.Mul2) & 0x00FF) >> 0;	break;
		case 0x206:	read = ((MMC5.Mul1 * MMC5.Mul2) & 0xFF00) >> 8;	break;
		}							break;
	case 0xC00:
	case 0xD00:
	case 0xE00:
	case 0xF00:	if (MMC5.GfxMode >= 2)
				read = MMC5.ExRAM[Addr & 0x3FF];	break;
	}
	return read;
}
void	_MAPINT	MMC5_CPUWrite5 (int Bank, int Addr, int Val)
{
	switch (Addr & 0xF00)
	{
	case 0x000:	MMC5sound_Write((Bank << 12) | Addr,Val);	break;
	case 0x100:
		switch (Addr)
		{
		case 0x100:	MMC5.PRGsize = Val & 3;
				MMC5_SyncPRG();		break;
		case 0x101:	MMC5.CHRsize = Val & 3;
				MMC5_SyncCHR(-1);	break;
		case 0x102:	MMC5.WRAMprot[0] = Val & 3;
							break;
		case 0x103:	MMC5.WRAMprot[1] = Val & 3;
							break;
		case 0x104:	MMC5.GfxMode = Val & 3;
				MMC5_SetPPUHandlers();
				MMC5_SyncMirror();	break;
		case 0x105:	MMC5.Mirror = Val;
				MMC5_SyncMirror();	break;
		case 0x106:	memset(MMC5.ExNameTable,Val,0x3C0);
							break;
		case 0x107:	memset(MMC5.ExNameTable+0x3C0,AttribBits[Val & 3],0x40);
							break;
		case 0x113:
		case 0x114:
		case 0x115:
		case 0x116:
		case 0x117:	MMC5.PRG[Addr - 0x113] = Val;
				MMC5_SyncPRG();		break;
		case 0x120:
		case 0x121:
		case 0x122:
		case 0x123:
		case 0x124:
		case 0x125:
		case 0x126:
		case 0x127:	MMC5.CHR_A[Addr & 0x7].b0 = Val;
				MMC5.CHR_A[Addr & 0x7].b1 = MMC5.CHRhi;
				MMC5.CHRmode = 0;
				MMC5_SyncCHR(-1);	break;
		case 0x128:
		case 0x129:
		case 0x12A:
		case 0x12B:	MMC5.CHR_B[Addr & 0x3].b0 = Val;
				MMC5.CHR_B[Addr & 0x3].b1 = MMC5.CHRhi;
				MMC5.CHRmode = 1;
				MMC5_SyncCHR(-1);	break;
		case 0x130:	MMC5.CHRhi = Val & 3;	break;
		}		break;
	case 0x200:
		switch (Addr)
		{
		case 0x200:	MMC5.SplitMode = Val;
				MMC5_SetPPUHandlers();	break;
		case 0x201:	MMC5.SplitScroll = Val;	break;
		case 0x202:	MMC5.SplitBank = Val;
				if (InSplitArea)
				{
					EMU->SetCHR_ROM4(0,MMC5.SplitBank);
					EMU->SetCHR_ROM4(4,MMC5.SplitBank);
				}			break;
		case 0x203:	MMC5.IRQline = Val;	break;
		case 0x204:	MMC5.IRQenabled = Val & 0x80;
				if (MMC5.IRQenabled & MMC5.IRQreads & 0x80)
					EMU->SetIRQ(0);
				else	EMU->SetIRQ(1);	break;
		case 0x205:	MMC5.Mul1 = Val;	break;
		case 0x206:	MMC5.Mul2 = Val;	break;
		}		break;
	case 0xC00:
	case 0xD00:
	case 0xE00:
	case 0xF00:	if (MMC5.GfxMode != 3)
				MMC5.ExRAM[Addr & 0x3FF] = Val;
							break;
	}
}

void	_MAPINT	MMC5_CPUWrite6F (int Bank, int Addr, int Val)
{
	if ((MMC5.WRAMprot[0] == 2) && (MMC5.WRAMprot[1] == 1))
		MMC5.CPUWrite6F(Bank,Addr,Val);
}

#ifdef	MMC5_EXTENDED_VSPLIT
int	_MAPINT	MMC5_PPUReadPT (int Bank, int Addr)
{
	if (!InSplitArea)
		return MMC5.PPURead[Bank](Bank,Addr);
	else	return MMC5.PPURead[Bank](Bank,(Addr & ~7) | (VScroll & 7));
}
#endif

static int extile = 0;
int	_MAPINT	MMC5_PPUReadNTSplitExt (int Bank, int Addr)
{
	if (CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Addr);	// sprite fetches can go through
	if (InSplitArea)
	{
		static int splittile;
		if (Addr < 0x3C0)
		{	// custom nametable data
			splittile = ((VScroll & 0xF8) << 2) | (CurTile & 0x1F);
			return MMC5.ExRAM[splittile];
		}	// custom attribute data
		else	return AttribBits[(MMC5.ExRAM[0x3C0 | AttribLoc[splittile >> 2]] >> AttribShift[splittile & 0x7F]) & 3];
	}
	else
	{
		if (Addr < 0x3C0)
		{
			extile = Addr;
			return MMC5.PPURead[Bank](Bank,Addr);		// normal nametable data
		}
		else
		{
			if (TileCache != (MMC5.ExRAM[extile] & 0x3F))
			{
				TileCache = (MMC5.ExRAM[extile] & 0x3F) | (MMC5.CHRhi << 6);
				EMU->SetCHR_ROM4(0,TileCache);
				EMU->SetCHR_ROM4(4,TileCache);
			}
			return AttribBits[MMC5.ExRAM[extile] >> 6];	// custom attribute data
		}
	}
}
int	_MAPINT	MMC5_PPUReadNTSplit (int Bank, int Addr)
{
	if (CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Addr);	// sprite fetches can go through
	if (InSplitArea)
	{
		static int splittile;
		if (Addr < 0x3C0)
		{	// custom nametable data
			splittile = ((VScroll & 0xF8) << 2) | (CurTile & 0x1F);
			return MMC5.ExRAM[splittile];
		}	// custom attribute data
		else	return AttribBits[(MMC5.ExRAM[0x3C0 | AttribLoc[splittile >> 2]] >> AttribShift[splittile & 0x7F]) & 3];
	}
	else	return MMC5.PPURead[Bank](Bank,Addr);	// weren't in split area, let it through
}
int	_MAPINT	MMC5_PPUReadNTExt (int Bank, int Addr)
{
	if (CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Addr);	// sprite fetches can go through
	if (Addr < 0x3C0)
	{
		extile = Addr;
		if (TileCache != (MMC5.ExRAM[extile] & 0x3F))
		{
			TileCache = (MMC5.ExRAM[extile] & 0x3F) | (MMC5.CHRhi << 6);
			EMU->SetCHR_ROM4(0,TileCache);
			EMU->SetCHR_ROM4(4,TileCache);
		}
		return MMC5.PPURead[Bank](Bank,Addr);		// normal nametable data
	}
	else	return AttribBits[MMC5.ExRAM[extile] >> 6];	// custom attribute data
}

static	void	MMC5_SetPPUHandlers (void)
{
	int x;
	if ((MMC5.SplitMode & 0x80) && (MMC5.GfxMode < 2))	// split mode
	{
#ifdef	MMC5_EXTENDED_VSPLIT
		for (x = 0; x < 8; x++)
			EMU->SetPPUReadHandler(x,MMC5_PPUReadPT);
#endif
		if (MMC5.GfxMode == 1)	
			for (x = 8; x < 0x10; x++)		// + exgfx
				EMU->SetPPUReadHandler(x,MMC5_PPUReadNTSplitExt);
		else
			for (x = 8; x < 0x10; x++)		// split only
				EMU->SetPPUReadHandler(x,MMC5_PPUReadNTSplit);
	}
	else if (MMC5.GfxMode == 1)				// exgfx only
	{
#ifdef	MMC5_EXTENDED_VSPLIT
		for (x = 0; x < 8; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
#endif
		for (x = 8; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5_PPUReadNTExt);
	}
	else							// normal
	{
#ifdef	MMC5_EXTENDED_VSPLIT
		for (x = 0; x < 8; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
#endif
		for (x = 8; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
	}
}

void	_MAPINT	MMC5_PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((Scanline == 240) && (Cycle == 0))
	{
		InSplitArea = FALSE;
		TileCache = -1;
		MMC5.LineCounter = -2;
		MMC5.IRQreads &= 0x80;
		MMC5_SyncCHR(-1);
	}
	if (!IsRendering)
		return;
	if (Cycle == 0)
	{
		if (Scanline == -1)
			MMC5.IRQreads |= 0x01;
		if (Scanline == 0)
		{
			MMC5.IRQreads &= ~0x80;
			EMU->SetIRQ(1);
		}
		if (Scanline == 1)
			MMC5.IRQreads |= 0x40;
		MMC5.LineCounter++;
		if ((MMC5.IRQline) && (MMC5.LineCounter == MMC5.IRQline))
			MMC5.IRQreads |= 0x80;
		if (MMC5.IRQenabled & MMC5.IRQreads & 0x80)
			EMU->SetIRQ(0);
	}
	if (Cycle == 256)
	{
		TileCache = -1;
		MMC5_SyncCHR(0);
	}
	else if (Cycle == 320)
	{
		CurTile = -1;
		if (Scanline == -1)
			VScroll = MMC5.SplitScroll;
		else if (Scanline < 240)
			VScroll++;
		if (VScroll >= 240)
			VScroll -= 240;
		MMC5_SyncCHR(1);
	}
	if ((!(Cycle & 7)) && (Cycle < 336))
	{
		CurTile++;
		if (MMC5.SplitMode & 0x80)
		{
			if (MMC5.SplitMode & 0x40)
			{
				if (CurTile == 0)
					InSplitArea = FALSE;
				else if (CurTile == (MMC5.SplitMode & 0x1F))
				{
					InSplitArea = TRUE;
					EMU->SetCHR_ROM4(0,MMC5.SplitBank);
					EMU->SetCHR_ROM4(4,MMC5.SplitBank);
				}
				else if (CurTile == 34)
				{
					InSplitArea = FALSE;
					MMC5_SyncCHR(0);
				}
			}
			else
			{
				if (CurTile == 0)
				{
					InSplitArea = TRUE;
					EMU->SetCHR_ROM4(0,MMC5.SplitBank);
					EMU->SetCHR_ROM4(4,MMC5.SplitBank);
				}
				else if (CurTile == (MMC5.SplitMode & 0x1F))
				{
					InSplitArea = FALSE;
					if (MMC5.GfxMode == 1)
						TileCache = -1;
					else	MMC5_SyncCHR(1);
				}
			}
		}
		else	InSplitArea = FALSE;
	}
}

int	_MAPINT	MMC5_MapperSnd (int Cycles)
{
	return MMC5sound_Get(Cycles);
}
