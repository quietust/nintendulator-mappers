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
			unsigned CHRbank : 6;
			unsigned PRG16   : 1;
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_HV  : 1;
			unsigned         : 2;
		};
		uint16 addr;
	};
	addr = Latch::Addr.s0;

	EMU->SetCHR_ROM8(0, CHRbank);
	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRGbank << 1) | (PRG16));
		EMU->SetPRG_ROM16(0xC, (PRGbank << 1) | (PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8, PRGbank);
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

uint8 MapperNum = 225;
} // namespace

const MapperInfo MapperInfo_225 =
{
	&MapperNum,
	_T("72-in-1"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_A,
	NULL,
	NULL
};