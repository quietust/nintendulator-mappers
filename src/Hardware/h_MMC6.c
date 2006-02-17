#include	"h_MMC6.h"

static	TMMC6	MMC6;

void	MMC6_Init (void (*Sync)(void))
{
	MMC6.PRG[0] = 0x00;	MMC6.PRG[1] = 0x01;	MMC6.PRG[2] = 0x3E;	MMC6.PRG[3] = 0x3F;	// 2 and 3 never change, simplifies GetPRGBank()

	MMC6.CHR[0] = 0x00;	MMC6.CHR[1] = 0x01;	MMC6.CHR[2] = 0x02;	MMC6.CHR[3] = 0x03;
	MMC6.CHR[4] = 0x04;	MMC6.CHR[5] = 0x05;	MMC6.CHR[6] = 0x06;	MMC6.CHR[7] = 0x07;

	MMC6.IRQenabled = MMC6.IRQcounter = MMC6.IRQlatch = 0;
	MMC6.Cmd = 0;
	MMC6.WriteWRAM = 0;
	MMC6.Mirror = 0;
	EMU->SetPRG_RAM8(0x6,0);
	MMC6.CPURead67 = EMU->GetCPUReadHandler(0x6);
	EMU->SetCPUReadHandler(0x6,MMC6_CPURead67);
	EMU->SetCPUReadHandler(0x7,MMC6_CPURead67);
	MMC6.CPUWrite67 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUWriteHandler(0x6,MMC6_CPUWrite67);
	EMU->SetCPUWriteHandler(0x7,MMC6_CPUWrite67);
	EMU->SetCPUWriteHandler(0x8,MMC6_CPUWrite89);
	EMU->SetCPUWriteHandler(0x9,MMC6_CPUWrite89);
	EMU->SetCPUWriteHandler(0xA,MMC6_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xB,MMC6_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xC,MMC6_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xD,MMC6_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xE,MMC6_CPUWriteEF);
	EMU->SetCPUWriteHandler(0xF,MMC6_CPUWriteEF);
	(MMC6.Sync = Sync)();
}

void	MMC6_Destroy (void)
{
	MMC6.Sync = NULL;
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

int	_MAPINT	MMC6_SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQlatch)
	SAVELOAD_BYTE(mode,x,data,MMC6.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,MMC6.Cmd)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,MMC6.PRG[i])
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
		SAVELOAD_BYTE(mode,x,data,MMC6.CHR[i])
	SAVELOAD_BYTE(mode,x,data,MMC6.WriteWRAM)
	SAVELOAD_BYTE(mode,x,data,MMC6.Mirror)
	if (mode == STATE_LOAD)
		MMC6.Sync();
	return x;
}

int	_MAPINT	MMC6_CPURead67 (int Bank, int Where)
{
	return MMC6.CPURead67(0x6,Where & 0x3FF);
}

void	_MAPINT	MMC6_CPUWrite67 (int Bank, int Where, int What)
{
//	if (MMC6.WriteWRAM & 0x80)	// it's more complicated than this, so we'll have to figure it out later
		MMC6.CPUWrite67(0x6,Where & 0x3FF,What);
}

void	_MAPINT	MMC6_CPUWrite89 (int Bank, int Where, int What)
{
	if (Where & 1)
		switch (MMC6.Cmd & 0x7)
		{
		case 0:	MMC6.CHR[0] = (What & 0xFE) | 0;
			MMC6.CHR[1] = (What & 0xFE) | 1;break;
		case 1:	MMC6.CHR[2] = (What & 0xFE) | 0;
			MMC6.CHR[3] = (What & 0xFE) | 1;break;
		case 2:	MMC6.CHR[4] = What;		break;
		case 3:	MMC6.CHR[5] = What;		break;
		case 4:	MMC6.CHR[6] = What;		break;
		case 5:	MMC6.CHR[7] = What;		break;
		case 6:	MMC6.PRG[0] = What & 0x3F;	break;
		case 7:	MMC6.PRG[1] = What & 0x3F;	break;
		}
	else	MMC6.Cmd = What;
	MMC6.Sync();
}

void	_MAPINT	MMC6_CPUWriteAB (int Bank, int Where, int What)
{
	if (Where & 1)
		MMC6.WriteWRAM = What;
	else	MMC6.Mirror = What;
	MMC6.Sync();
}

void	_MAPINT	MMC6_CPUWriteCD (int Bank, int Where, int What)
{
	if (Where & 1)
		MMC6.IRQlatch = What;
	else	MMC6.IRQcounter = MMC6.IRQlatch = What;
	EMU->SetIRQ(1);
}

void	_MAPINT	MMC6_CPUWriteEF (int Bank, int Where, int What)
{
	MMC6.IRQenabled = (Where & 1);
	EMU->SetIRQ(1);
}

void	_MAPINT	MMC6_PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((MMC6.IRQenabled) && (IsRendering) && (Cycle == 262))
	{
		if (!MMC6.IRQcounter)
		{
			MMC6.IRQcounter = MMC6.IRQlatch;
			EMU->SetIRQ(0);
		}
		else	MMC6.IRQcounter--;
	}
}