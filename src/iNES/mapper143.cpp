/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPURead _Read4;

int	MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x100)
		return (~Addr & 0x3F) | ((*EMU->OpenBus) & 0xC0);
	else if (Bank == 4)
		return _Read4(Bank, Addr);
	else	return -1;
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, Read);
	EMU->SetCPUReadHandler(0x5, Read);

	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, 0);
}

uint8 MapperNum = 143;
} // namespace

const MapperInfo MapperInfo_143 =
{
	&MapperNum,
	_T("Sachen (TC-A001-72P/SA-014)"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};