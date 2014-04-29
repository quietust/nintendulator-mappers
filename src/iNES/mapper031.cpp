/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper015.cpp $
 * $Id: mapper015.cpp 1173 2011-01-27 04:24:05Z quietust $
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG[8];

void	Sync (void)
{
	for (int i = 0; i < 8; i++)
		EMU->SetPRG_ROM4(0x8 | i, PRG[i]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	PRG[Addr & 7] = Val;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0, 0);
	else	EMU->SetCHR_RAM8(0, 0);

	EMU->SetCPUWriteHandler(0x5, Write);

	if (ResetType == RESET_HARD)
		PRG[7] = 0xFF;
	Sync();
}

uint8 MapperNum = 31;
} // namespace

const MapperInfo MapperInfo_031 =
{
	&MapperNum,
	_T("2A03 Puritans Album"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};