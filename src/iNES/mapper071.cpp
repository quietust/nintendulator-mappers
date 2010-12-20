/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	WriteCDEF (int Bank, int Addr, int Val)
{
	PRG = Val;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Val & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x9, Write9);	/* Fire Hawk needs this */
	EMU->SetCPUWriteHandler(0xC, WriteCDEF);
	EMU->SetCPUWriteHandler(0xD, WriteCDEF);
	EMU->SetCPUWriteHandler(0xE, WriteCDEF);
	EMU->SetCPUWriteHandler(0xF, WriteCDEF);

	if (ResetType == RESET_HARD)
		PRG = 0;

	Sync();
}

uint8 MapperNum = 71;
} // namespace

const MapperInfo MapperInfo_071 =
{
	&MapperNum,
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
};