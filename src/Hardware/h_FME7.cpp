/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_FME7.h"
#include	"Sound\s_SUN5.h"

namespace FME7
{
uint8_t IRQenabled;
uint16_n IRQcounter;
uint8_t Cmd;
uint8_t PRG[4], CHR[8];
uint8_t Mirror;
FSync Sync;
BOOL SyncOnLoad;

void	Load (FSync _Sync, BOOL _SyncOnLoad)
{
	SUN5sound::Load();
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		Cmd = 0;

		PRG[0] = 0x00;	PRG[1] = 0x00;	PRG[2] = 0x01;	PRG[3] = 0xFE;

		CHR[0] = 0x00;	CHR[1] = 0x01;	CHR[2] = 0x02;	CHR[3] = 0x03;
		CHR[4] = 0x04;	CHR[5] = 0x05;	CHR[6] = 0x06;	CHR[7] = 0x07;

		Mirror = 0;

		IRQenabled = 0;
		IRQcounter.s0 = 0;
	}
	
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCDEF);
	EMU->SetCPUWriteHandler(0xD, WriteCDEF);
	EMU->SetCPUWriteHandler(0xE, WriteCDEF);
	EMU->SetCPUWriteHandler(0xF, WriteCDEF);

	SUN5sound::Reset(ResetType);
	Sync();
}

void	Unload (void)
{
	SUN5sound::Unload();
}

void	SyncMirror (void)
{
	switch (Mirror)
	{
	case 0:	EMU->Mirror_A10();	break;
	case 1:	EMU->Mirror_A11();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

void	SyncPRG (int AND, int OR)
{
	if (PRG[0] & 0x40)
		if (PRG[0] & 0x80)
			EMU->SetPRG_RAM8(0x6, 0);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
	else	EMU->SetPRG_ROM8(0x6, (PRG[0] & 0x3F & AND) | OR);
	EMU->SetPRG_ROM8(0x8, (PRG[1] & AND) | OR);
	EMU->SetPRG_ROM8(0xA, (PRG[2] & AND) | OR);
	EMU->SetPRG_ROM8(0xC, (PRG[3] & AND) | OR);
	EMU->SetPRG_ROM8(0xE, (0xFF & AND) | OR);
}

void	SyncCHR (int AND, int OR)
{
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, (CHR[i] & AND) | OR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	CheckSave(offset = SUN5sound::SaveLoad(mode, offset, data));

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	Cmd = Val & 0xF;
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Cmd)
	{
	case 0x0:	CHR[0] = Val;	break;
	case 0x1:	CHR[1] = Val;	break;
	case 0x2:	CHR[2] = Val;	break;
	case 0x3:	CHR[3] = Val;	break;
	case 0x4:	CHR[4] = Val;	break;
	case 0x5:	CHR[5] = Val;	break;
	case 0x6:	CHR[6] = Val;	break;
	case 0x7:	CHR[7] = Val;	break;
	case 0x8:	PRG[0] = Val;	break;
	case 0x9:	PRG[1] = Val & 0x3F;	break;
	case 0xA:	PRG[2] = Val & 0x3F;	break;
	case 0xB:	PRG[3] = Val & 0x3F;	break;
	case 0xC:	Mirror = Val & 3;	break;
	case 0xD:	IRQenabled = Val & 0x81;
			EMU->SetIRQ(1);		break;
	case 0xE:	IRQcounter.b0 = Val;	break;
	case 0xF:	IRQcounter.b1 = Val;	break;
	}
	Sync();
}

void	MAPINT	WriteCDEF (int Bank, int Addr, int Val)
{
	SUN5sound::Write((Bank << 12) | Addr,Val);
	Sync();
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled & 0x80) && (!--IRQcounter.s0) && (IRQenabled & 0x01))
		EMU->SetIRQ(0);
}

int	MAPINT	GenSound (int Cycles)
{
	return SUN5sound::Get(Cycles);
}
} // namespace FME7
