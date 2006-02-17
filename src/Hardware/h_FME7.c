#include	"h_FME07.h"
#include	"Sound\s_FME07.h"

static	TFME07	FME07;

void	FME07_Init (void (*Sync)(void))
{
	FME07.Cmd = 0;

	FME07.PRG[0] = 0x00;	FME07.PRG[1] = 0x00;	FME07.PRG[2] = 0x01;	FME07.PRG[3] = 0xFE;

	FME07.CHR[0] = 0x00;	FME07.CHR[1] = 0x01;	FME07.CHR[2] = 0x02;	FME07.CHR[3] = 0x03;
	FME07.CHR[4] = 0x04;	FME07.CHR[5] = 0x05;	FME07.CHR[6] = 0x06;	FME07.CHR[7] = 0x07;

	FME07.Mirror = 0;

	FME07.IRQenabled = 0;
	FME07.IRQcounter.s0 = 0;
	
	EMU->SetCPUWriteHandler(0x8,FME07_Write89);
	EMU->SetCPUWriteHandler(0x9,FME07_Write89);
	EMU->SetCPUWriteHandler(0xA,FME07_WriteAB);
	EMU->SetCPUWriteHandler(0xB,FME07_WriteAB);
	EMU->SetCPUWriteHandler(0xC,FME07_WriteCDEF);
	EMU->SetCPUWriteHandler(0xD,FME07_WriteCDEF);
	EMU->SetCPUWriteHandler(0xE,FME07_WriteCDEF);
	EMU->SetCPUWriteHandler(0xF,FME07_WriteCDEF);

	FME07sound_Init();
	(FME07.Sync = Sync)();
}

void	FME07_Destroy (void)
{
	FME07sound_Destroy();
}

void	FME07_SyncMirror (void)
{
	switch (FME07.Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

void	FME07_SyncPRG (int AND, int OR)
{
	if (FME07.PRG[0] & 0x40)
		if (FME07.PRG[0] & 0x80)
			EMU->SetPRG_RAM8(0x6,0);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
	else	EMU->SetPRG_ROM8(0x6,(FME07.PRG[0] & 0x3F & AND) | OR);
	EMU->SetPRG_ROM8(0x8,(FME07.PRG[1] & AND) | OR);
	EMU->SetPRG_ROM8(0xA,(FME07.PRG[2] & AND) | OR);
	EMU->SetPRG_ROM8(0xC,(FME07.PRG[3] & AND) | OR);
	EMU->SetPRG_ROM8(0xE,(0xFF & AND) | OR);
}

void	FME07_SyncCHR (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,(FME07.CHR[x] & AND) | OR);
}

int	_MAPINT	FME07_SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,FME07.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,FME07.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,FME07.Cmd)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,FME07.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,FME07.CHR[i])
	SAVELOAD_BYTE(mode,x,data,FME07.Mirror)
	x = FME07sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		FME07.Sync();
	return x;
}

void	_MAPINT	FME07_Write89 (int Bank, int Where, int What)
{
	FME07.Cmd = What & 0xF;
}

void	_MAPINT	FME07_WriteAB (int Bank, int Where, int What)
{
	switch (FME07.Cmd)
	{
	case 0x0:	FME07.CHR[0] = What;	break;
	case 0x1:	FME07.CHR[1] = What;	break;
	case 0x2:	FME07.CHR[2] = What;	break;
	case 0x3:	FME07.CHR[3] = What;	break;
	case 0x4:	FME07.CHR[4] = What;	break;
	case 0x5:	FME07.CHR[5] = What;	break;
	case 0x6:	FME07.CHR[6] = What;	break;
	case 0x7:	FME07.CHR[7] = What;	break;
	case 0x8:	FME07.PRG[0] = What;	break;
	case 0x9:	FME07.PRG[1] = What & 0x3F;	break;
	case 0xA:	FME07.PRG[2] = What & 0x3F;	break;
	case 0xB:	FME07.PRG[3] = What & 0x3F;	break;
	case 0xC:	FME07.Mirror = What & 3;	break;
	case 0xD:	FME07.IRQenabled = What;
			EMU->SetIRQ(1);			break;
	case 0xE:	FME07.IRQcounter.b0 = What;
			EMU->SetIRQ(1);			break;
	case 0xF:	FME07.IRQcounter.b1 = What;
			EMU->SetIRQ(1);			break;
	}
	FME07.Sync();
}

void	_MAPINT	FME07_WriteCDEF (int Bank, int Where, int What)
{
	FME07sound_Write((Bank << 12) | Where,What);
	FME07.Sync();
}

void	_MAPINT	FME07_CPUCycle (void)
{
	if ((FME07.IRQenabled) && (!FME07.IRQcounter.s0--))
		EMU->SetIRQ(0);
}

int	_MAPINT	FME07_GenSound (int Cycles)
{
	return FME07sound_Get(Cycles);
}
