#include	"h_MMC6.h"

static	TMMC6	MMC6;

void	MMC6_Load (FSync Sync)
{
	MMC6.Sync = Sync;
}

void	MMC6_Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		MMC6.PRG[0] = 0x00;	MMC6.PRG[1] = 0x01;	MMC6.PRG[2] = 0x3E;	MMC6.PRG[3] = 0x3F;	// 2 and 3 never change, simplifies GetPRGBank()

		MMC6.CHR[0] = 0x00;	MMC6.CHR[1] = 0x01;	MMC6.CHR[2] = 0x02;	MMC6.CHR[3] = 0x03;
		MMC6.CHR[4] = 0x04;	MMC6.CHR[5] = 0x05;	MMC6.CHR[6] = 0x06;	MMC6.CHR[7] = 0x07;

		MMC6.IRQenabled = MMC6.IRQcounter = MMC6.IRQlatch = 0;
		MMC6.Cmd = 0;
		MMC6.WRAMEnab = 0;
		MMC6.Mirror = 0;
	}
	EMU->SetPRG_RAM4(0x7,0);
	MMC6.CPURead7 = EMU->GetCPUReadHandler(0x7);
	EMU->SetCPUReadHandler(0x7,MMC6_CPURead7);
	MMC6.CPUWrite7 = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x7,MMC6_CPUWrite7);
	EMU->SetCPUWriteHandler(0x8,MMC6_CPUWrite89);
	EMU->SetCPUWriteHandler(0x9,MMC6_CPUWrite89);
	EMU->SetCPUWriteHandler(0xA,MMC6_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xB,MMC6_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xC,MMC6_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xD,MMC6_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xE,MMC6_CPUWriteEF);
	EMU->SetCPUWriteHandler(0xF,MMC6_CPUWriteEF);
	MMC6.Sync();
}

void	MMC6_Unload (void)
{
}

void	MMC6_SyncMirror (void)
{
	if (MMC6.Mirror & 1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MMC6_GetPRGBank (int Bank)
{
	if (Bank & 0x1)
		return MMC6.PRG[Bank];
	else	return MMC6.PRG[Bank ^ ((MMC6.Cmd & 0x40) >> 5)];
}

int	MMC6_GetCHRBank (int Bank)
{
	return MMC6.CHR[Bank ^ ((MMC6.Cmd & 0x80) >> 5)];
}

void	MMC6_SyncPRG (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),(MMC6_GetPRGBank(x) & AND) | OR);
}

void	MMC6_SyncCHR_ROM (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,(MMC6_GetCHRBank(x) & AND) | OR);
}

void	MMC6_SyncCHR_RAM (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_RAM1(x,(MMC6_GetCHRBank(x) & AND) | OR);
}

int	MAPINT	MMC6_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQcounter);
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQlatch);
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQenabled);
	SAVELOAD_BYTE(mode,x,data,MMC6.Cmd);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,MMC6.PRG[i]);
	switch (mode)
	{
	case STATE_SAVE:
		data[x++] = MMC6.CHR[0];
		data[x++] = MMC6.CHR[2];
		break;
	case STATE_LOAD:
		MMC6.CHR[0] = data[x++];
		MMC6.CHR[1] = MMC6.CHR[0] | 1;
		MMC6.CHR[2] = data[x++];
		MMC6.CHR[3] = MMC6.CHR[2] | 1;
		break;
	case STATE_SIZE:
		x += 2;
		break;
	}
	for (i = 4; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,MMC6.CHR[i]);
	SAVELOAD_BYTE(mode,x,data,MMC6.WRAMEnab);
	SAVELOAD_BYTE(mode,x,data,MMC6.Mirror);
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQreload);
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQaddr);
	if (mode == STATE_LOAD)
		MMC6.Sync();
	return x;
}

int	MAPINT	MMC6_CPURead7 (int Bank, int Addr)
{
	if (MMC6.WRAMEnab & 0xA0)
	{
		if ((MMC6.WRAMEnab >> ((Addr & 0x200) >> 8)) & 0x20)
			return MMC6.CPURead7(0x7,Addr & 0x3FF);
		else	return 0;
	}
	else	return -1;
}

void	MAPINT	MMC6_CPUWrite7 (int Bank, int Addr, int Val)
{
	if (((MMC6.WRAMEnab >> ((Addr & 0x200) >> 8)) & 0x30) == 0x30)
		MMC6.CPUWrite7(0x7,Addr & 0x3FF,Val);
}

void	MAPINT	MMC6_CPUWrite89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (MMC6.Cmd & 0x7)
		{
		case 0:	MMC6.CHR[0] = (Val & 0xFE) | 0;
			MMC6.CHR[1] = (Val & 0xFE) | 1;break;
		case 1:	MMC6.CHR[2] = (Val & 0xFE) | 0;
			MMC6.CHR[3] = (Val & 0xFE) | 1;break;
		case 2:	MMC6.CHR[4] = Val;		break;
		case 3:	MMC6.CHR[5] = Val;		break;
		case 4:	MMC6.CHR[6] = Val;		break;
		case 5:	MMC6.CHR[7] = Val;		break;
		case 6:	MMC6.PRG[0] = Val & 0x3F;	break;
		case 7:	MMC6.PRG[1] = Val & 0x3F;	break;
		}
	else
	{
		MMC6.Cmd = Val;
		if (Val & 0x20)
			MMC6.WRAMEnab |= 1;
	}
	MMC6.Sync();
}

void	MAPINT	MMC6_CPUWriteAB (int Bank, int Addr, int Val)
{
	if (Addr & 1)
	{
		if (MMC6.WRAMEnab & 1)
			MMC6.WRAMEnab = Val | 1;
	}
	else	MMC6.Mirror = Val;
	MMC6.Sync();
}

void	MAPINT	MMC6_CPUWriteCD (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC6.IRQreload = 1;
	else	MMC6.IRQlatch = Val;
}

void	MAPINT	MMC6_CPUWriteEF (int Bank, int Addr, int Val)
{
	MMC6.IRQenabled = (Addr & 1);
	if (!MMC6.IRQenabled)
		EMU->SetIRQ(1);
}
void	MAPINT	MMC6_PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (MMC6.IRQaddr)
		MMC6.IRQaddr--;
	if ((!MMC6.IRQaddr) && (Addr & 0x1000))
	{
		if (!MMC6.IRQcounter || MMC6.IRQreload)
		{
			MMC6.IRQcounter = MMC6.IRQlatch;
			MMC6.IRQreload = 0;
		}
		else	MMC6.IRQcounter--;
		if (!MMC6.IRQcounter && MMC6.IRQenabled)
			EMU->SetIRQ(0);
	}
	if (Addr & 0x1000)
		MMC6.IRQaddr = 8;
}