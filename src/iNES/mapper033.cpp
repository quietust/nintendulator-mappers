/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG[2];
uint8_t CHR[6];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM2(0x0, CHR[0]);
	EMU->SetCHR_ROM2(0x2, CHR[1]);
	EMU->SetCHR_ROM1(0x4, CHR[2]);
	EMU->SetCHR_ROM1(0x5, CHR[3]);
	EMU->SetCHR_ROM1(0x6, CHR[4]);
	EMU->SetCHR_ROM1(0x7, CHR[5]);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	PRG[0] = Val & 0x1F;
		Mirror = Val & 0x40;	break;
	case 1:	PRG[1] = Val & 0x1F;	break;
	case 2:	CHR[0] = Val;		break;
	case 3:	CHR[1] = Val;		break;
	}
	Sync();
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[2] = Val;	break;
	case 1:	CHR[3] = Val;	break;
	case 2:	CHR[4] = Val;	break;
	case 3:	CHR[5] = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;
		PRG[1] = 1;
		for (int i = 0; i < 6; i++)
			CHR[i] = i;
		Mirror = 0;
	}
	Sync();
}

uint16_t MapperNum = 33;
} // namespace

const MapperInfo MapperInfo_033
(
	&MapperNum,
	_T("Taito TC0190"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
