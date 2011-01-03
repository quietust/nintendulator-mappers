/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRG     : 4;
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned         : 1;
			unsigned Mir_HV  : 1;
			unsigned         : 8;
		};
		uint16 addr;
	};
	addr = Latch::Addr.s0;
	EMU->SetCHR_RAM8(0, 0);
	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRG << 1) | PRG16);
		EMU->SetPRG_ROM16(0xC, (PRG << 1) | PRG16);
	}
	else	EMU->SetPRG_ROM32(0x8, PRG);
	if (Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

void	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint8 MapperNum = 61;
} // namespace

const MapperInfo MapperInfo_061 =
{
	&MapperNum,
	_T("20-in-1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_A,
	NULL,
	NULL
};