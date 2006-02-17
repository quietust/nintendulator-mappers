#include	"h_MMC5.h"
#include	"Sound\s_MMC5.h"

TMMC5	MMC5;

#define	MMC5_EXTENDED_VSPLIT	/* equivalent to selecting 'SL' mode on a cartridge rather than 'CL' mode */

static	void	MMC5_SetPPUHandlers (void);

void	MMC5_Init (void)
{
	u8 x;

	EMU->SetCPUReadHandler(0x5,MMC5_CPURead5);
	EMU->SetCPUWriteHandler(0x5,MMC5_CPUWrite5);
	MMC5.CPUWrite6F = EMU->GetCPUWriteHandler(0x8);
	for (x = 6; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,MMC5_CPUWrite6F);
	for (x = 0; x < 16; x++)
		MMC5.PPURead[x] = EMU->GetPPUReadHandler(x);
	
	EMU->Mirror_4();
	MMC5.NameTable0		= EMU->GetCHR_Ptr1(0x8);
	MMC5.NameTable1		= EMU->GetCHR_Ptr1(0x9);
	MMC5.ExRAM		= EMU->GetCHR_Ptr1(0xA);
	MMC5.ExNameTable	= EMU->GetCHR_Ptr1(0xB);

	MMC5.PRGsize = 3;
	MMC5.CHRsize = 0;
	
	MMC5.IRQenabled = MMC5.IRQreads = MMC5.IRQline = 0;
	MMC5.Mul1 = MMC5.Mul2 = MMC5.GfxMode = 0;
	
	MMC5.SplitMode = MMC5.SplitBank = MMC5.SplitScroll = 0;
	MMC5_SetPPUHandlers();

	for (x = 0; x < 8; x++)	MMC5.CHR_A[x] = x;
	for (x = 0; x < 4; x++)	MMC5.CHR_B[x] = x;
	for (x = 0; x < 2; x++)	MMC5.WRAMprot[x] = 0;
	for (x = 0; x < 4; x++)
	{
		u8 y = x | 4;
		MMC5.WRAMtable[MMC5WRAM_0KB_0KB][x] = 
		MMC5.WRAMtable[MMC5WRAM_0KB_8KB][x] =
		MMC5.WRAMtable[MMC5WRAM_0KB_32KB][x] = -1;

		MMC5.WRAMtable[MMC5WRAM_0KB_0KB][y] =
		MMC5.WRAMtable[MMC5WRAM_8KB_0KB][y] =
		MMC5.WRAMtable[MMC5WRAM_32KB_0KB][y] = -1;

		MMC5.WRAMtable[MMC5WRAM_8KB_0KB][x] =
		MMC5.WRAMtable[MMC5WRAM_8KB_8KB][x] =
		MMC5.WRAMtable[MMC5WRAM_8KB_32KB][x] = 0;

		MMC5.WRAMtable[MMC5WRAM_0KB_8KB][y] =
		MMC5.WRAMtable[MMC5WRAM_8KB_8KB][y] =
		MMC5.WRAMtable[MMC5WRAM_32KB_8KB][y] = 4;

		MMC5.WRAMtable[MMC5WRAM_32KB_0KB][x] =
		MMC5.WRAMtable[MMC5WRAM_32KB_8KB][x] =
		MMC5.WRAMtable[MMC5WRAM_32KB_32KB][x] = x;
	
		MMC5.WRAMtable[MMC5WRAM_0KB_32KB][y] =
		MMC5.WRAMtable[MMC5WRAM_8KB_32KB][y] =
		MMC5.WRAMtable[MMC5WRAM_32KB_32KB][y] = y;
	}

	for (x = 0; x < 5; x++)	MMC5.PRG[x] = 0xFF;

	MMC5.TileCache = 0x40;
	MMC5sound_Init();
	MMC5_SyncPRG();
}

void	MMC5_Destroy (void)
{
	MMC5sound_Destroy();
}

int	_MAPINT	MMC5_SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,MMC5.PRGsize)
	SAVELOAD_BYTE(mode,x,data,MMC5.CHRsize)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQreads)
	SAVELOAD_BYTE(mode,x,data,MMC5.IRQline)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mul1)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mul2)
	SAVELOAD_BYTE(mode,x,data,MMC5.GfxMode)
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.CHR_A[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.CHR_B[i])
	for (i = 0; i < 5; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.PRG[i])
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,MMC5.WRAMprot[i])
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitMode)
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitBank)
	SAVELOAD_BYTE(mode,x,data,MMC5.SplitScroll)
	SAVELOAD_BYTE(mode,x,data,MMC5.Mirror)
	x = MMC5sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
	{
		MMC5_SyncPRG();
		MMC5_SyncCHR();
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
			if (MMC5.WRAMtable[MMC5.WRAMsize][Bank & 0x7] == -1)
				for (x = Loc; x < Loc + 2; x++)
					EMU->SetPRG_OB4(x);
			else	EMU->SetPRG_RAM8(Loc,MMC5.WRAMtable[MMC5.WRAMsize][Bank & 0x7]);
		}
		else if (Size == 16)
		{
			if (MMC5.WRAMtable[MMC5.WRAMsize][Bank & 0x6] == -1)
				for (x = Loc; x < Loc + 4; x++)
					EMU->SetPRG_OB4(x);
			else
			{
				EMU->SetPRG_RAM8(Loc+0,MMC5.WRAMtable[MMC5.WRAMsize][(Bank & 0x6) | 0]);
				EMU->SetPRG_RAM8(Loc+2,MMC5.WRAMtable[MMC5.WRAMsize][(Bank & 0x6) | 1]);
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

int LastCHR = 0;

void	MMC5_SyncCHRA (void)
{
	switch (MMC5.CHRsize)
	{
	case 0:	EMU->SetCHR_ROM8(0,MMC5.CHR_A[7]);	break;
	case 1:	EMU->SetCHR_ROM4(0,MMC5.CHR_A[3]);
		EMU->SetCHR_ROM4(4,MMC5.CHR_A[7]);	break;
	case 2:	EMU->SetCHR_ROM2(0,MMC5.CHR_A[1]);
		EMU->SetCHR_ROM2(2,MMC5.CHR_A[3]);
		EMU->SetCHR_ROM2(4,MMC5.CHR_A[5]);
		EMU->SetCHR_ROM2(6,MMC5.CHR_A[7]);	break;
	case 3:	EMU->SetCHR_ROM1(0,MMC5.CHR_A[0]);
		EMU->SetCHR_ROM1(1,MMC5.CHR_A[1]);
		EMU->SetCHR_ROM1(2,MMC5.CHR_A[2]);
		EMU->SetCHR_ROM1(3,MMC5.CHR_A[3]);
		EMU->SetCHR_ROM1(4,MMC5.CHR_A[4]);
		EMU->SetCHR_ROM1(5,MMC5.CHR_A[5]);
		EMU->SetCHR_ROM1(6,MMC5.CHR_A[6]);
		EMU->SetCHR_ROM1(7,MMC5.CHR_A[7]);	break;
	}
	LastCHR = 0;
}

void	MMC5_SyncCHRB (void)
{
	switch (MMC5.CHRsize)
	{
	case 0:	EMU->SetCHR_ROM8(0,MMC5.CHR_B[3]);	break;
	case 1:	EMU->SetCHR_ROM4(0,MMC5.CHR_B[3]);
		EMU->SetCHR_ROM4(4,MMC5.CHR_B[3]);	break;
	case 2:	EMU->SetCHR_ROM2(0,MMC5.CHR_B[1]);
		EMU->SetCHR_ROM2(2,MMC5.CHR_B[3]);
		EMU->SetCHR_ROM2(4,MMC5.CHR_B[1]);
		EMU->SetCHR_ROM2(6,MMC5.CHR_B[3]);	break;
	case 3:	EMU->SetCHR_ROM1(0,MMC5.CHR_B[0]);
		EMU->SetCHR_ROM1(1,MMC5.CHR_B[1]);
		EMU->SetCHR_ROM1(2,MMC5.CHR_B[2]);
		EMU->SetCHR_ROM1(3,MMC5.CHR_B[3]);
		EMU->SetCHR_ROM1(4,MMC5.CHR_B[0]);
		EMU->SetCHR_ROM1(5,MMC5.CHR_B[1]);
		EMU->SetCHR_ROM1(6,MMC5.CHR_B[2]);
		EMU->SetCHR_ROM1(7,MMC5.CHR_B[3]);	break;
	}
	LastCHR = 1;
}

void	MMC5_SyncCHR (void)
{
	if (LastCHR)
		MMC5_SyncCHRB();
	else	MMC5_SyncCHRA();
}

void	MMC5_SyncMirror (void)
{
	EMU->Mirror_Custom(
		(MMC5.Mirror >> 0) & 3,
		(MMC5.Mirror >> 2) & 3,
		(MMC5.Mirror >> 4) & 3,
		(MMC5.Mirror >> 6) & 3);	
}

int	_MAPINT	MMC5_CPURead5 (int Bank, int Where)
{
	register u8 read = -1;
	switch (Where & 0xF00)
	{
	case 0x200:
		switch (Where)
		{
		case 0x204:	read = MMC5.IRQreads;
				MMC5.IRQreads &= 0x40;
				EMU->SetIRQ(1);					break;
		case 0x205:	read = ((MMC5.Mul1 * MMC5.Mul2) & 0x00FF) >> 0;	break;
		case 0x206:	read = ((MMC5.Mul1 * MMC5.Mul2) & 0xFF00) >> 8;	break;
		}							break;
	case 0xC00:
	case 0xD00:
	case 0xE00:
	case 0xF00:	if (MMC5.GfxMode >= 2)
				read = MMC5.ExRAM[Where & 0x3FF];	break;
	}
	return read;
}

void	_MAPINT	MMC5_CPUWrite5 (int Bank, int Where, int What)
{
	switch (Where & 0xF00)
	{
	case 0x000:	MMC5sound_Write((Bank << 12) | Where,What);	break;
	case 0x100:
		switch (Where)
		{
		case 0x100:	MMC5.PRGsize = What & 3;
				MMC5_SyncPRG();		break;
		case 0x101:	MMC5.CHRsize = What & 3;
				MMC5_SyncCHR();		break;
		case 0x102:	MMC5.WRAMprot[0] = What & 3;
							break;
		case 0x103:	MMC5.WRAMprot[1] = What & 3;
							break;
		case 0x104:	MMC5.GfxMode = What & 3;
				MMC5_SetPPUHandlers();	break;
		case 0x105:	MMC5.Mirror = What;
				MMC5_SyncMirror();	break;
		case 0x106:	memset(MMC5.ExNameTable,What,0x3C0);
							break;
		case 0x107:	memset(MMC5.ExNameTable+0x3C0,((What & 3) |
				((What & 3) << 2) | ((What & 3) << 4) |
				((What & 3) << 6)),0x3C);
							break;
		case 0x113:
		case 0x114:
		case 0x115:
		case 0x116:
		case 0x117:	MMC5.PRG[Where - 0x113] = What;
				MMC5_SyncPRG();		break;
		case 0x120:
		case 0x121:
		case 0x122:
		case 0x123:
		case 0x124:
		case 0x125:
		case 0x126:
		case 0x127:	MMC5.CHR_A[Where & 0x7] = What;
				MMC5_SyncCHRA();	break;
		case 0x128:
		case 0x129:
		case 0x12A:
		case 0x12B:	MMC5.CHR_B[Where & 0x3] = What;
				MMC5_SyncCHRB();	break;
		case 0x130:	/* nobody knows... */	break;
		}		break;
	case 0x200:
		switch (Where)
		{
		case 0x200:	MMC5.SplitMode = What;
				MMC5_SetPPUHandlers();	break;
		case 0x201:	MMC5.SplitScroll = What;break;
		case 0x202:	MMC5.SplitBank = What;	break;
		case 0x203:	MMC5.IRQline = What;
				EMU->SetIRQ(1);		break;
		case 0x204:	MMC5.IRQenabled = What & 0x80;
				EMU->SetIRQ(1);		break;
		case 0x205:	MMC5.Mul1 = What;	break;
		case 0x206:	MMC5.Mul2 = What;	break;
		}		break;
	case 0xC00:
	case 0xD00:
	case 0xE00:
	case 0xF00:	if (MMC5.GfxMode != 3)
				MMC5.ExRAM[Where & 0x3FF] = What;
							break;
	}
}

void	_MAPINT	MMC5_CPUWrite6F (int Bank, int Where, int What)
{
	if ((MMC5.WRAMprot[0] == 2) && (MMC5.WRAMprot[1] == 1))
		MMC5.CPUWrite6F(Bank,Where,What);
}

static	BOOL	InSplitArea = FALSE;
static	int	VScroll;

#ifdef	MMC5_EXTENDED_VSPLIT
int	_MAPINT	MMC5_PPUReadPT (int Bank, int Where)
{
	if (!InSplitArea)
		return MMC5.PPURead[Bank](Bank,Where);
	else	return MMC5.PPURead[Bank](Bank,(Where & ~7) | (VScroll & 7));
}
#endif

static	unsigned char	AttribBits[4] = {0x00,0x55,0xAA,0xFF};
const	unsigned char	AttribLoc[256] =
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
const	unsigned char	AttribShift[128] =
{
	0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
	4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6,4,4,6,6
};

static int extile = 0;
int	_MAPINT	MMC5_PPUReadNTSplitExt (int Bank, int Where)
{
	if (MMC5.CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Where);	// sprite fetches can go through
	if (InSplitArea)
	{
		static int splittile;
		if (Where < 0x3C0)
		{	// custom nametable data
			splittile = ((VScroll & 0xF8) << 2) | (MMC5.CurTile & 0x1F);
			return MMC5.ExRAM[splittile];
		}	// custom attribute data
		else	return AttribBits[(MMC5.ExRAM[0x3C0 | AttribLoc[splittile >> 2]] >> AttribShift[splittile & 0x7F]) & 3];
	}
	else
	{
		if (Where < 0x3C0)
		{
			extile = Where;
			return MMC5.PPURead[Bank](Bank,Where);		// normal nametable data
		}
		else
		{
			if (MMC5.TileCache != (MMC5.ExRAM[extile] & 0x3F))
			{
				MMC5.TileCache = MMC5.ExRAM[extile] & 0x3F;
				EMU->SetCHR_ROM4(Bank & 4,MMC5.TileCache);
			}
			return AttribBits[MMC5.ExRAM[extile] >> 6];	// custom attribute data
		}
	}
}
int	_MAPINT	MMC5_PPUReadNTSplit (int Bank, int Where)
{
	if (MMC5.CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Where);	// sprite fetches can go through
	if (InSplitArea)
	{
		static int splittile;
		if (Where < 0x3C0)
		{	// custom nametable data
			splittile = ((VScroll & 0xF8) << 2) | (MMC5.CurTile & 0x1F);
			return MMC5.ExRAM[splittile];
		}	// custom attribute data
		else	return AttribBits[(MMC5.ExRAM[0x3C0 | AttribLoc[splittile >> 2]] >> AttribShift[splittile & 0x7F]) & 3];
	}
	else	return MMC5.PPURead[Bank](Bank,Where);	// weren't in split area, let it through
}
int	_MAPINT	MMC5_PPUReadNTExt (int Bank, int Where)
{
	if (MMC5.CurTile >= 34)				// we only want to deal with background data
		return MMC5.PPURead[Bank](Bank,Where);	// sprite fetches can go through
	if (Where < 0x3C0)
	{
		extile = Where;
		if (MMC5.TileCache != (MMC5.ExRAM[extile] & 0x3F))
		{
			MMC5.TileCache = MMC5.ExRAM[extile] & 0x3F;
//			EMU->SetCHR_ROM4(Bank & 4,MMC5.TileCache);	// this works with everything but "Shin 4 Nin Uchi Mahjong - Yakuman Tengoku"
			EMU->SetCHR_ROM4(0,MMC5.TileCache);
			EMU->SetCHR_ROM4(4,MMC5.TileCache);
		}
		return MMC5.PPURead[Bank](Bank,Where);		// normal nametable data
	}
	else	return AttribBits[MMC5.ExRAM[extile] >> 6];	// custom attribute data
}

static	void	MMC5_SetPPUHandlers (void)
{
	int x;
#ifdef	MMC5_EXTENDED_VSPLIT
	if ((MMC5.SplitMode & 0x80) && (MMC5.GfxMode < 2))	// split mode
	{
		for (x = 0; x < 8; x++)
			EMU->SetPPUReadHandler(x,MMC5_PPUReadPT);
		if (MMC5.GfxMode == 1)	
			for (x = 8; x < 0x10; x++)		// + exgfx
				EMU->SetPPUReadHandler(x,MMC5_PPUReadNTSplitExt);
		else
			for (x = 8; x < 0x10; x++)		// split only
				EMU->SetPPUReadHandler(x,MMC5_PPUReadNTSplit);
	}
	else if (MMC5.GfxMode == 1)				// exgfx only
	{
		for (x = 0; x < 8; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
		for (x = 8; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5_PPUReadNTExt);
	}
	else							// normal
	{
		for (x = 0; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
	}
#else
	if (((MMC5.GfxMode < 2) && (MMC5.SplitMode & 0x80)) || (MMC5.GfxMode == 1))	// split mode and/or exgfx
	{
		for (x = 8; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5_PPUReadNTSplitExt);
	}
	else							// normal
	{
		for (x = 8; x < 0x10; x++)
			EMU->SetPPUReadHandler(x,MMC5.PPURead[x]);
	}
#endif
}

void	_MAPINT	MMC5_PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	static int LineCounter = 0;
	if (!IsRendering)
		return;
	if (Cycle == 256)
		MMC5_SyncCHRA();
	else if (Cycle == 320)
	{
		MMC5.TileCache = 0x40;
		MMC5.CurTile = -1;
		if (Scanline == -1)
			VScroll = MMC5.SplitScroll + 1;
		else if (Scanline < 240)
			VScroll++;
		if (VScroll >= 240)
			VScroll -= 240;
		MMC5_SyncCHRB();
		LineCounter++;
		if (LineCounter == 240)
			MMC5.IRQreads &= ~0x40;
		if (LineCounter == MMC5.IRQline)
			MMC5.IRQreads |= 0x80;
		if ((MMC5.IRQreads & 0x80) && (MMC5.IRQenabled & 0x80))
			EMU->SetIRQ(0);
	}
	else if ((Scanline == 239) && (Cycle == 338))
	{
		MMC5.CurTile = 0;
		LineCounter = -1;
		MMC5.IRQreads = 0x40;
		MMC5_SyncCHRA();
	}
	if ((!(Cycle & 7)) && (Cycle < 336))
		MMC5.CurTile++;
	if (MMC5.SplitMode & 0x80)
	{
		if (MMC5.SplitMode & 0x40)
		{
			if (MMC5.CurTile == 0)
				InSplitArea = FALSE;
			else if (MMC5.CurTile == (MMC5.SplitMode & 0x1F))
			{
				InSplitArea = TRUE;
				EMU->SetCHR_ROM4(0,MMC5.SplitBank);
				EMU->SetCHR_ROM4(4,MMC5.SplitBank);
			}
			else if (MMC5.CurTile == 34)
				InSplitArea = FALSE;
		}
		else
		{
			if (MMC5.CurTile == 0)
			{
				InSplitArea = TRUE;
				EMU->SetCHR_ROM4(0,MMC5.SplitBank);
				EMU->SetCHR_ROM4(4,MMC5.SplitBank);
			}
			else if (MMC5.CurTile == (MMC5.SplitMode & 0x1F))
			{
				InSplitArea = FALSE;
				if (MMC5.GfxMode == 1)
					MMC5.TileCache = 0x40;
				else	MMC5_SyncCHRB();
			}
		}
	}
	else	InSplitArea = FALSE;
}

int	_MAPINT	MMC5_MapperSnd (int Cycles)
{
	return MMC5sound_Get(Cycles);
}
