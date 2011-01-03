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
	EMU->SetPRG_ROM32(0x8, (Reg & 0x38) >> 3);
	EMU->SetCHR_ROM8(0, (Reg & 0x7) | ((Reg & 0x40) >> 3));
	if (Reg & 0x80)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
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
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if (Addr & 0x100)
	{
		Reg = Val;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x6; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;

	Sync();
}

uint8 MapperNum = 113;
} // namespace

const MapperInfo MapperInfo_113 =
{
	&MapperNum,
	_T("Mapper 113 (HES)"),
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