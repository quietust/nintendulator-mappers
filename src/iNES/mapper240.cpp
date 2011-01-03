/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Reg;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0xF0) >> 4);
	EMU->SetCHR_ROM8(0, Reg & 0xF);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Reg);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if ((Bank == 4) && (Addr < 0x020))
	{
		_Write4(Bank, Addr, Val);
		return;
	}
	Reg = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x6; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}

uint8 MapperNum = 240;
} // namespace

const MapperInfo MapperInfo_240 =
{
	&MapperNum,
	_T("Mapper 240"),
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