/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 Reg;
FCPURead _Read4;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0, Reg & 0x3);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Reg);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

int	MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x100)
		return ((*EMU->OpenBus) & 0xF0) | Reg;
	else if (Bank == 4)
		return _Read4(Bank, Addr);
	else	return -1;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if ((Addr & 0x103) == 0x102)
		Reg = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	_Read4 = EMU->GetCPUReadHandler(0x4);
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
	{
		EMU->SetCPUReadHandler(x, Read);
		EMU->SetCPUWriteHandler(x, Write);
	}

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}

u8 MapperNum = 132;
} // namespace

CTMapperInfo	MapperInfo_132 =
{
	&MapperNum,
	_T("Creatom"),
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
