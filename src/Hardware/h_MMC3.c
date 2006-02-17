#include	"h_MMC3.h"

static	TMMC3	MMC3;

void	MMC3_Init (void (*Sync)(void))
{
	MMC3.PRG[0] = 0x3C;	MMC3.PRG[1] = 0x3D;	MMC3.PRG[2] = 0x3E;	MMC3.PRG[3] = 0x3F;	// 2 and 3 never change, simplifies GetPRGBank()

	MMC3.CHR[0] = 0x00;	MMC3.CHR[1] = 0x01;	MMC3.CHR[2] = 0x02;	MMC3.CHR[3] = 0x03;
	MMC3.CHR[4] = 0x04;	MMC3.CHR[5] = 0x05;	MMC3.CHR[6] = 0x06;	MMC3.CHR[7] = 0x07;

	MMC3.IRQenabled = MMC3.IRQcounter = MMC3.IRQlatch = 0;
	MMC3.Cmd = 0;
	if (ROM->ROMType == ROM_INES)
		MMC3.WRAMEnab = 0x80;
	else	MMC3.WRAMEnab = 0;
	if (ROM->ROMType == ROM_INES)
		MMC3.Mirror = (ROM->INES_Flags & 0x01) ? 0 : 1;
	else	MMC3.Mirror = 0;
	MMC3.CPUWrite67 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUWriteHandler(0x6,MMC3_CPUWrite67);
	EMU->SetCPUWriteHandler(0x7,MMC3_CPUWrite67);
	EMU->SetCPUWriteHandler(0x8,MMC3_CPUWrite89);
	EMU->SetCPUWriteHandler(0x9,MMC3_CPUWrite89);
	EMU->SetCPUWriteHandler(0xA,MMC3_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xB,MMC3_CPUWriteAB);
	EMU->SetCPUWriteHandler(0xC,MMC3_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xD,MMC3_CPUWriteCD);
	EMU->SetCPUWriteHandler(0xE,MMC3_CPUWriteEF);
	EMU->SetCPUWriteHandler(0xF,MMC3_CPUWriteEF);
	(MMC3.Sync = Sync)();
}

void	MMC3_Destroy (void)
{
	MMC3.Sync = NULL;
}

void	MMC3_SyncMirror (void)
{
	if (MMC3.Mirror & 1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MMC3_GetPRGBank (int Bank)
{
	if (Bank & 0x1)
		return MMC3.PRG[Bank];
	else	return MMC3.PRG[Bank ^ ((MMC3.Cmd & 0x40) >> 5)];
}

int	MMC3_GetCHRBank (int Bank)
{
	return MMC3.CHR[Bank ^ ((MMC3.Cmd & 0x80) >> 5)];
}

void	MMC3_SyncPRG (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),(MMC3_GetPRGBank(x) & AND) | OR);
}

void	MMC3_SyncWRAM (void)
{
	if (MMC3.WRAMEnab & 0x80)
		EMU->SetPRG_RAM8(0x6,0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}

void	MMC3_SyncCHR_ROM (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,(MMC3_GetCHRBank(x) & AND) | OR);
}

void	MMC3_SyncCHR_RAM (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_RAM1(x,(MMC3_GetCHRBank(x) & AND) | OR);
}

int	_MAPINT	MMC3_SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,MMC3.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,MMC3.IRQlatch)
	SAVELOAD_BYTE(mode,x,data,MMC3.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,MMC3.Cmd)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,MMC3.PRG[i])
	switch (mode)
	{
	case STATE_SAVE:
		data[x++] = MMC3.CHR[0];
		data[x++] = MMC3.CHR[2];
		break;
	case STATE_LOAD:
		MMC3.CHR[0] = data[x++];
		MMC3.CHR[1] = MMC3.CHR[0] | 1;
		MMC3.CHR[2] = data[x++];
		MMC3.CHR[3] = MMC3.CHR[2] | 1;
		break;
	case STATE_SIZE:
		x += 2;
		break;
	}
	for (i = 4; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,MMC3.CHR[i])
	SAVELOAD_BYTE(mode,x,data,MMC3.WRAMEnab)
	SAVELOAD_BYTE(mode,x,data,MMC3.Mirror)
	SAVELOAD_BYTE(mode,x,data,MMC3.IRQreload)
	SAVELOAD_BYTE(mode,x,data,MMC3.IRQaddr)
	if (mode == STATE_LOAD)
		MMC3.Sync();
	return x;
}

void	_MAPINT	MMC3_CPUWrite67 (int Bank, int Where, int What)
{
	if (!(MMC3.WRAMEnab & 0x40))
		MMC3.CPUWrite67(Bank,Where,What);
}

void	_MAPINT	MMC3_CPUWrite89 (int Bank, int Where, int What)
{
	if (Where & 1)
		switch (MMC3.Cmd & 0x7)
		{
		case 0:	MMC3.CHR[0] = (What & 0xFE) | 0;
			MMC3.CHR[1] = (What & 0xFE) | 1;break;
		case 1:	MMC3.CHR[2] = (What & 0xFE) | 0;
			MMC3.CHR[3] = (What & 0xFE) | 1;break;
		case 2:	MMC3.CHR[4] = What;		break;
		case 3:	MMC3.CHR[5] = What;		break;
		case 4:	MMC3.CHR[6] = What;		break;
		case 5:	MMC3.CHR[7] = What;		break;
		case 6:	MMC3.PRG[0] = What & 0x3F;	break;
		case 7:	MMC3.PRG[1] = What & 0x3F;	break;
		}
	else	MMC3.Cmd = What;
	MMC3.Sync();
}

void	_MAPINT	MMC3_CPUWriteAB (int Bank, int Where, int What)
{
	if (Where & 1)
		MMC3.WRAMEnab = (ROM->ROMType == ROM_INES) ? 0x80 : What;
	else	MMC3.Mirror = What;
	MMC3.Sync();
}

void	_MAPINT	MMC3_CPUWriteCD (int Bank, int Where, int What)
{
	if (Where & 1)
		MMC3.IRQreload = 1;
	else	MMC3.IRQlatch = What;
}

void	_MAPINT	MMC3_CPUWriteEF (int Bank, int Where, int What)
{
	MMC3.IRQenabled = (Where & 1);
	if (!MMC3.IRQenabled)
		EMU->SetIRQ(1);
}
void	_MAPINT	MMC3_PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (MMC3.IRQaddr)
		MMC3.IRQaddr--;
	if ((!MMC3.IRQaddr) && (Addr & 0x1000))
	{
		unsigned char count = MMC3.IRQcounter;
		if (!count || MMC3.IRQreload)
		{
			MMC3.IRQcounter = MMC3.IRQlatch;
			MMC3.IRQreload = 0;
		}
		else	MMC3.IRQcounter--;
		if (count && !MMC3.IRQcounter && MMC3.IRQenabled)
			EMU->SetIRQ(0);
	}
	if (Addr & 0x1000)
		MMC3.IRQaddr = 4;
}
