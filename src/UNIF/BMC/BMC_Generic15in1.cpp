/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
u8 Data;

void	Sync (void)
{
	if (Data & 0x0E)
	{
		EMU->SetPRG_ROM16(0x8, 0x0F);
		EMU->SetPRG_ROM16(0xC, Data & 0x0F);
	}
	else	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, Data & 0x0F);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Data = EMU->GetCPUReadHandler(Bank)(Bank, Addr);
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x, Write);
	Data = 0;
	UNIF_SetMirroring(NULL);
	Sync();
}
} // namespace

CTMapperInfo	MapperInfo_BMC_Generic15in1 =
{
	"BMC-Generic15in1",
	_T("Pirate multicart mapper"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};