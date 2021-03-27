/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG;
uint8_t CHR[3];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG);
	EMU->SetPRG_ROM8(0xA, -3);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM4(0x0, CHR[0] >> 2);
	EMU->SetCHR_ROM2(0x4, CHR[1] >> 1);
	EMU->SetCHR_ROM2(0x6, CHR[2] >> 1);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[0] = Val;	break;
	case 1:	CHR[1] = Val;	break;
	case 2:	CHR[2] = Val;	break;
	case 3:	PRG = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		CHR[0] = CHR[1] = CHR[2] = 0;
	}
	Sync();
}

uint16_t MapperNum = 193;
} // namespace

const MapperInfo MapperInfo_193
(
	&MapperNum,
	_T("Fighting Hero"),
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
