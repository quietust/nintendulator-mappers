/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
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
			unsigned CHRbank : 4;
			unsigned         : 1;
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 4;
			unsigned PRGchip : 2;
			unsigned Mir_HV  : 1;
			unsigned         : 2;
		};
		u16 addr;
	};
	u8 openbus = 0;
	addr = Latch::Addr.s0;

	if (Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	
	switch (PRGchip)
	{
	case 2:	openbus = 1;	break;
	case 3:	PRGchip = 2;	break;
	}
	if (openbus)
	{
		for (openbus = 0x8; openbus <= 0xF; openbus++)
			EMU->SetPRG_OB4(openbus);
	}
	else
	{
		if (PRGsize)
		{
			EMU->SetPRG_ROM16(0x8, (PRGchip << 5) | (PRGbank << 1) | (PRG16));
			EMU->SetPRG_ROM16(0xC, (PRGchip << 5) | (PRGbank << 1) | (PRG16));
		}
		else EMU->SetPRG_ROM32(0x8, (PRGchip << 4) | (PRGbank));
	}
	EMU->SetCHR_ROM8(0, (CHRbank << 2) | (Latch::Data & 0x3));
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

u8 MapperNum = 228;
} // namespace

const MapperInfo MapperInfo_228 =
{
	&MapperNum,
	_T("Action 52"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_AD,
	NULL,
	NULL
};