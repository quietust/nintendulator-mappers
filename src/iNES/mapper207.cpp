/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG[3], CHR[6];
uint8_t NT[2];
FCPUWrite _Write7;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, -1);
	EMU->SetCHR_ROM2(0x0, CHR[0] >> 1);
	EMU->SetCHR_ROM2(0x2, CHR[1] >> 1);
	EMU->SetCHR_ROM1(0x4, CHR[2]);
	EMU->SetCHR_ROM1(0x5, CHR[3]);
	EMU->SetCHR_ROM1(0x6, CHR[4]);
	EMU->SetCHR_ROM1(0x7, CHR[5]);
	EMU->Mirror_Custom(NT[0] >> 7, NT[0] >> 7, NT[1] >> 7, NT[1] >> 7);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, NT[i]);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	_Write7(Bank, Addr, Val);
	switch (Addr)
	{
	case 0xEF0:	CHR[0] = Val & 0x7F;
			NT[0] = Val & 0x80;	break;
	case 0xEF1:	CHR[1] = Val & 0x7F;
			NT[1] = Val & 0x80;	break;
	case 0xEF2:	CHR[2] = Val;		break;
	case 0xEF3:	CHR[3] = Val;		break;
	case 0xEF4:	CHR[4] = Val;		break;
	case 0xEF5:	CHR[5] = Val;		break;
	case 0xEFA:
	case 0xEFB:	PRG[0] = Val;		break;
	case 0xEFC:
	case 0xEFD:	PRG[1] = Val;		break;
	case 0xEFE:
	case 0xEFF:	PRG[2] = Val;		break;
	}
	Sync();
}

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write7 = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
	{
		NT[0] = NT[1] = 0;
		for (int i = 0; i < 8; i++)
			CHR[i] = i;
		PRG[0] = 0;
		PRG[1] = 1;
		PRG[2] = 0xFE;
	}
	Sync();
}

uint16_t MapperNum = 207;
} // namespace

const MapperInfo MapperInfo_207
(
	&MapperNum,
	_T("Mapper 207"),
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
