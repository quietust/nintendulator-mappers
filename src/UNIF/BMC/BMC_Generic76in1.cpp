/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
uint8 Regs[2];

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

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, Regs[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Regs[Addr & 1] = Val;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);
	for (int i = 0; i < 2; i++)
		Regs[i] = 0;
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