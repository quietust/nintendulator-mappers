/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	union
	{
		struct
		{
			unsigned         : 1;
			unsigned PRG     : 4;
			unsigned PRGsize : 1;
			unsigned Mirror  : 2;
			unsigned         : 8;
		};
		u16 addr;
	};
	addr = Latch::Addr.s0;
	
	EMU->SetCHR_RAM8(0, 0);
	if (PRGsize)
		EMU->SetPRG_ROM32(0x8, PRG);
	else
	{
		EMU->SetPRG_ROM16(0x8, PRG << 1);
		EMU->SetPRG_ROM16(0x8, PRG << 1);
	}
	switch (Mirror)
	{
	case 0:	EMU->Mirror_S0();		break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_H();		break;
	case 3:	EMU->Mirror_Custom(0, 1, 1, 1);	break;
	}
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
} // namespace

CTMapperInfo	MapperInfo_BMC_Generic20in1 =
{
	"BMC-Generic20in1",
	_T("Pirate multicart mapper"),
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