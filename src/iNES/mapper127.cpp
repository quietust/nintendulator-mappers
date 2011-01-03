/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG[4];
uint8 CHR[8];
uint8 IRQenabled;
uint8 IRQcounter;
uint8 Mirror[4];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, PRG[3]);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i]);
	for (int i = 0; i < 4; i++)
	{
		EMU->SetCHR_NT1(0x8 | i, Mirror[i]);
		EMU->SetCHR_NT1(0xC | i, Mirror[i]);
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, Mirror[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled) && (!--IRQcounter))
		EMU->SetIRQ(0);
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x73)
	{
	case 0x00:	PRG[0] = Val & 0xF;		break;
	case 0x01:	PRG[1] = Val & 0xF;		break;
	case 0x02:	PRG[2] = Val & 0xF;		break;
	case 0x03:	PRG[3] = (Val & 0x3) | 0xC;	break;
	case 0x10:	CHR[0] = Val & 0x7F;		break;
	case 0x11:	CHR[1] = Val & 0x7F;		break;
	case 0x12:	CHR[2] = Val & 0x7F;		break;
	case 0x13:	CHR[3] = Val & 0x7F;		break;
	case 0x20:	CHR[4] = Val & 0x7F;		break;
	case 0x21:	CHR[5] = Val & 0x7F;		break;
	case 0x22:	CHR[6] = Val & 0x7F;		break;
	case 0x23:	CHR[7] = Val & 0x7F;		break;
	case 0x30:	case 0x31:	case 0x32:	case 0x33:
			IRQenabled = 1;			break;
	case 0x40:	case 0x41:	case 0x42:	case 0x43:
			IRQenabled = 0;
			IRQcounter = 0;
			EMU->SetIRQ(1);				break;
	case 0x50:	Mirror[0] = Val & 1;		break;
	case 0x51:	Mirror[1] = Val & 1;		break;
	case 0x52:	Mirror[2] = Val & 1;		break;
	case 0x53:	Mirror[3] = Val & 1;		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
	{
		for (int i = 0; i < 4; i++)
			PRG[i] = 0xF;
		for (int i = 0; i < 8; i++)
			CHR[i] = 0;
		IRQenabled = 0;
		IRQcounter = 0;
		for (int i = 0; i < 4; i++)
			Mirror[i] = 0;
	}
	Sync();
}

uint8 MapperNum = 127;
} // namespace

const MapperInfo MapperInfo_127 =
{
	&MapperNum,
	_T("Double Dragon pirate"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
