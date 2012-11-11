/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 regSel;
uint8 regVal[4];

void	Sync (void)
{
	EMU->SetCHR_RAM8(0, regVal[0] & 0x3);
	switch (regVal[2] & 0x3)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_V();	break;
	case 3:	EMU->Mirror_H();	break;
	}

	int bank32 = regVal[3], bank16 = regVal[3] << 1;
	switch ((regVal[2] & 0x30) >> 4)
	{
	case 0:	bank32 = (bank32 & 0x3F) | (regVal[1] & 0x0);
		bank16 = (bank16 & 0x7E) | (regVal[1] & 0x1);
		break;
	case 1:	bank32 = (bank32 & 0x3E) | (regVal[1] & 0x1);
		bank16 = (bank16 & 0x7C) | (regVal[1] & 0x3);
		break;
	case 2:	bank32 = (bank32 & 0x3C) | (regVal[1] & 0x3);
		bank16 = (bank16 & 0x78) | (regVal[1] & 0x7);
		break;
	case 3:	bank32 = (bank32 & 0x38) | (regVal[1] & 0x7);
		bank16 = (bank16 & 0x70) | (regVal[1] & 0xF);
		break;
	}
	switch ((regVal[2] & 0x0C) >> 2)
	{
	case 0:
	case 1:	EMU->SetPRG_ROM32(0x8, bank32);
		break;
	case 2:	EMU->SetPRG_ROM16(0x8, (regVal[3] << 1) | 0);
		EMU->SetPRG_ROM16(0xC, bank16);
		break;
	case 3:	EMU->SetPRG_ROM16(0x8, bank16);
		EMU->SetPRG_ROM16(0xC, (regVal[3] << 1) | 1);
		break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, regSel);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, regVal[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	WriteLow (int Bank, int Addr, int Val)
{
	regSel = Val;
}
void	MAPINT	WriteHigh (int Bank, int Addr, int Val)
{
	switch (regSel & 0x81)
	{
	case 0x00:
		regVal[0] = Val & 0x03;
		break;
	case 0x01:
		regVal[1] = Val & 0x0F;
		break;
	case 0x80:
		regVal[2] = Val & 0x3F;
		break;
	case 0x81:
		regVal[3] = Val & 0x3F;
		break;
	}
	if (!(regSel & 0x80) && !(regVal[2] & 0x2))
		regVal[2] = (regVal[2] & 0xFE) | ((Val & 0x10) >> 4);
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x5, WriteLow);
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, WriteHigh);

	if (ResetType == RESET_HARD)
	{
		regSel = 0;
		regVal[0] = 0x3;
		regVal[1] = 0xF;
		regVal[2] = 0x3F;
		regVal[3] = 0x3F;
	}

	Sync();
}

uint8 MapperNum = 28;
} // namespace

const MapperInfo MapperInfo_028 =
{
	&MapperNum,
	_T("Action 53"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};