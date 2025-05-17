/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG;
uint16_n CHR[8];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);

	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i].s0);
	switch (Mirror)
	{
	case 0:	EMU->Mirror_A10();	break;
	case 1:	EMU->Mirror_A11();	break;
	case 2: // Actual functionality unknown
	case 3:	EMU->Mirror_S0();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 8; i++)
		SAVELOAD_WORD(mode, offset, data, CHR[i].s0);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x1F)
	{
	case 0x0:	CHR[0].b0 = Val;	break;
	case 0x1:	CHR[1].b0 = Val;	break;
	case 0x2:	CHR[2].b0 = Val;	break;
	case 0x3:	CHR[3].b0 = Val;	break;
	case 0x4:	CHR[0].b1 = Val;	break;
	case 0x5:	CHR[1].b1 = Val;	break;
	case 0x6:	CHR[2].b1 = Val;	break;
	case 0x7:	CHR[3].b1 = Val;	break;
	case 0x8:	CHR[4].b0 = Val;	break;
	case 0x9:	CHR[5].b0 = Val;	break;
	case 0xA:	CHR[6].b0 = Val;	break;
	case 0xB:	CHR[7].b0 = Val;	break;
	case 0xC:	CHR[4].b1 = Val;	break;
	case 0xD:	CHR[5].b1 = Val;	break;
	case 0xE:	CHR[6].b1 = Val;	break;
	case 0xF:	CHR[7].b1 = Val;	break;
	case 0x10:	PRG = Val;		break;
	case 0x14:	Mirror = Val & 0x1;	break;
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
	EMU->SetCPUWriteHandler(0xC, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = 0x00;
		for (int i = 0; i < 8; i++)
			CHR[i].s0 = i;
		// Actual mirroring unknown - other implementations start with 1-screen,
		// then switch to H/V when writing to $C014
		Mirror = 3;
	}
	Sync();
}
} // namespace

const MapperInfo MapperInfo_156
(
	156,
	_T("DISC23C01 DAOU"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
