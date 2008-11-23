/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_FME7.h"
#include	"Sound\s_FME7.h"

typedef	struct	FME7
{
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 Cmd;
	u8 PRG[4], CHR[8];
	u8 Mirror;
	FSync Sync;
}	TFME7, *PFME7;
static	TFME7	FME7;

void	FME7_Load (FSync Sync)
{
	FME7sound_Load();
	FME7.Sync = Sync;
}

void	FME7_Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		FME7.Cmd = 0;

		FME7.PRG[0] = 0x00;	FME7.PRG[1] = 0x00;	FME7.PRG[2] = 0x01;	FME7.PRG[3] = 0xFE;

		FME7.CHR[0] = 0x00;	FME7.CHR[1] = 0x01;	FME7.CHR[2] = 0x02;	FME7.CHR[3] = 0x03;
		FME7.CHR[4] = 0x04;	FME7.CHR[5] = 0x05;	FME7.CHR[6] = 0x06;	FME7.CHR[7] = 0x07;

		FME7.Mirror = 0;

		FME7.IRQenabled = 0;
		FME7.IRQcounter.s0 = 0;
	}
	
	EMU->SetCPUWriteHandler(0x8,FME7_Write89);
	EMU->SetCPUWriteHandler(0x9,FME7_Write89);
	EMU->SetCPUWriteHandler(0xA,FME7_WriteAB);
	EMU->SetCPUWriteHandler(0xB,FME7_WriteAB);
	EMU->SetCPUWriteHandler(0xC,FME7_WriteCDEF);
	EMU->SetCPUWriteHandler(0xD,FME7_WriteCDEF);
	EMU->SetCPUWriteHandler(0xE,FME7_WriteCDEF);
	EMU->SetCPUWriteHandler(0xF,FME7_WriteCDEF);

	FME7sound_Reset(ResetType);
	FME7.Sync();
}

void	FME7_Unload (void)
{
	FME7sound_Unload();
}

void	FME7_SyncMirror (void)
{
	switch (FME7.Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

void	FME7_SyncPRG (int AND, int OR)
{
	if (FME7.PRG[0] & 0x40)
		if (FME7.PRG[0] & 0x80)
			EMU->SetPRG_RAM8(0x6,0);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
	else	EMU->SetPRG_ROM8(0x6,(FME7.PRG[0] & 0x3F & AND) | OR);
	EMU->SetPRG_ROM8(0x8,(FME7.PRG[1] & AND) | OR);
	EMU->SetPRG_ROM8(0xA,(FME7.PRG[2] & AND) | OR);
	EMU->SetPRG_ROM8(0xC,(FME7.PRG[3] & AND) | OR);
	EMU->SetPRG_ROM8(0xE,(0xFF & AND) | OR);
}

void	FME7_SyncCHR (int AND, int OR)
{
	u8 x;
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,(FME7.CHR[x] & AND) | OR);
}

int	MAPINT	FME7_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,FME7.IRQcounter.s0);
	SAVELOAD_BYTE(mode,x,data,FME7.IRQenabled);
	SAVELOAD_BYTE(mode,x,data,FME7.Cmd);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,FME7.PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,FME7.CHR[i]);
	SAVELOAD_BYTE(mode,x,data,FME7.Mirror);
	x = FME7sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		FME7.Sync();
	return x;
}

void	MAPINT	FME7_Write89 (int Bank, int Addr, int Val)
{
	FME7.Cmd = Val & 0xF;
}

void	MAPINT	FME7_WriteAB (int Bank, int Addr, int Val)
{
	switch (FME7.Cmd)
	{
	case 0x0:	FME7.CHR[0] = Val;	break;
	case 0x1:	FME7.CHR[1] = Val;	break;
	case 0x2:	FME7.CHR[2] = Val;	break;
	case 0x3:	FME7.CHR[3] = Val;	break;
	case 0x4:	FME7.CHR[4] = Val;	break;
	case 0x5:	FME7.CHR[5] = Val;	break;
	case 0x6:	FME7.CHR[6] = Val;	break;
	case 0x7:	FME7.CHR[7] = Val;	break;
	case 0x8:	FME7.PRG[0] = Val;	break;
	case 0x9:	FME7.PRG[1] = Val & 0x3F;	break;
	case 0xA:	FME7.PRG[2] = Val & 0x3F;	break;
	case 0xB:	FME7.PRG[3] = Val & 0x3F;	break;
	case 0xC:	FME7.Mirror = Val & 3;	break;
	case 0xD:	FME7.IRQenabled = Val & 0x81;
			if (FME7.IRQenabled != 0x81)
				EMU->SetIRQ(1);		break;
	case 0xE:	FME7.IRQcounter.b0 = Val;	break;
	case 0xF:	FME7.IRQcounter.b1 = Val;	break;
	}
	FME7.Sync();
}

void	MAPINT	FME7_WriteCDEF (int Bank, int Addr, int Val)
{
	FME7sound_Write((Bank << 12) | Addr,Val);
	FME7.Sync();
}

void	MAPINT	FME7_CPUCycle (void)
{
	if ((FME7.IRQenabled & 0x80) && (!--FME7.IRQcounter.s0) && (FME7.IRQenabled & 0x01))
		EMU->SetIRQ(0);
}

int	MAPINT	FME7_GenSound (int Cycles)
{
	return FME7sound_Get(Cycles);
}
