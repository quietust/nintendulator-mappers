/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG;
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_RAM8(0x0, 0);
	if (Mirror & 0x80)
		iNES_SetMirroring();
	else if (Mirror & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mirror = Val & 0x10;
	Sync();
}
void	MAPINT	WriteCDEF (int Bank, int Addr, int Val)
{
	PRG = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x9, Write9);	/* Fire Hawk needs this */
	EMU->SetCPUWriteHandler(0xC, WriteCDEF);
	EMU->SetCPUWriteHandler(0xD, WriteCDEF);
	EMU->SetCPUWriteHandler(0xE, WriteCDEF);
	EMU->SetCPUWriteHandler(0xF, WriteCDEF);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		Mirror = 0x80;
	}

	Sync();
}
} // namespace

const MapperInfo MapperInfo_071
(
	71,
	_T("Camerica BF9093/BF9097"),
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
