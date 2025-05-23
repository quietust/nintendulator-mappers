/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t regSel;
uint8_t regVal[4];

void	Sync (void)
{
	EMU->SetCHR_RAM8(0x0, regVal[0] & 0x3);
	switch (regVal[2] & 0x3)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_A10();	break;
	case 3:	EMU->Mirror_A11();	break;
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
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, regSel);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, regVal[i]);

	if (IsLoad(mode))
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
} // namespace

const MapperInfo MapperInfo_028
(
	28,
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
);
