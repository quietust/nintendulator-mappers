/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t BankReg, Mode;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0); // For compatibility with mapper hacks
	switch (Mode)
	{
	case 0:
		EMU->SetPRG_ROM32(0x8, (BankReg & 0x3E) >> 1);
		break;
	case 1:
		EMU->SetPRG_ROM16(0x8, (BankReg & 0x3F));
		EMU->SetPRG_ROM16(0xC, (BankReg & 0x38) | 0x07);
		break;
	case 2:
		EMU->SetPRG_ROM8(0x8, ((BankReg & 0x3F) << 1) | ((BankReg & 0x80) >> 7));
		EMU->SetPRG_ROM8(0xA, ((BankReg & 0x3F) << 1) | ((BankReg & 0x80) >> 7));
		EMU->SetPRG_ROM8(0xC, ((BankReg & 0x3F) << 1) | ((BankReg & 0x80) >> 7));
		EMU->SetPRG_ROM8(0xE, ((BankReg & 0x3F) << 1) | ((BankReg & 0x80) >> 7));
		break;
	case 3:
		EMU->SetPRG_ROM16(0x8, (BankReg & 0x3F));
		EMU->SetPRG_ROM16(0xC, (BankReg & 0x3F));
		break;
	}
	EMU->SetCHR_RAM8(0x0, 0);	// For compatibility with mapper hacks, do not write-protect
	if (BankReg & 0x40)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	// Translate old savestate data
	if (ver == 0)
	{
		uint8_t PRG[4];
		uint8_t Mirror;

		for (int i = 0; i < 4; i++)
			SAVELOAD_BYTE(mode, offset, data, PRG[i]);
		SAVELOAD_BYTE(mode, offset, data, Mirror);

		BankReg = ((PRG[0] >> 1) & 0x3F) | Mirror;
		if (PRG[0] == PRG[1])
		{
			Mode = 2;
			BankReg |= (PRG[0] & 1) << 7;
		}
		else if (PRG[0] == PRG[2])
			Mode = 3;
		else if (PRG[2] == (PRG[0] | 2))
			Mode = 0;
		else	Mode = 1;
	}
	else
	{
		SAVELOAD_BYTE(mode, offset, data, BankReg);
		SAVELOAD_BYTE(mode, offset, data, Mode);
	}

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Mode = (Addr & 0x3);
	BankReg = Val;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		BankReg = Mode = 0;
	Sync();
}

uint16_t MapperNum = 15;
} // namespace

const MapperInfo MapperInfo_015
(
	&MapperNum,
	_T("K-1029 (100-in-1 Contra Function 16)"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
