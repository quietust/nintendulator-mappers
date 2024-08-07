/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
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
		uint16_t addr;
	};
	addr = Latch::Addr.s0;
	
	EMU->SetCHR_RAM8(0x0, 0);
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
	case 1:	EMU->Mirror_A10();		break;
	case 2:	EMU->Mirror_A11();		break;
	case 3:	EMU->Mirror_Custom(0, 1, 1, 1);	break;
	}
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, TRUE);
	return TRUE;
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

const MapperInfo MapperInfo_BMC_Generic20in1
(
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
);
