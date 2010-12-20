/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
u8 Regs[2];

void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_VH  : 1;
			unsigned PRGhi   : 2;
			unsigned         : 7;
		};
		struct
		{
			unsigned byte0   : 8;
			unsigned byte1   : 8;
		};
	};

	byte0 = Regs[0];
	byte1 = Regs[1];

	if (Mir_VH)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, ((PRGhi) << 5) | (PRGbank));
		EMU->SetPRG_ROM16(0xC, ((PRGhi) << 5) | (PRGbank));
	}
	else	EMU->SetPRG_ROM32(0x8, ((PRGhi) << 4) | (PRGbank >> 1));
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, Regs[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Regs[Addr & 1] = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x, Write);
	for (x = 0; x < 2; x++)
		Regs[x] = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_BMC_Generic76in1 =
{
	"BMC-Generic76in1",
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