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
			unsigned CHRbank : 6;
			unsigned PRG16   : 1;
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_A11 : 1;
			unsigned         : 2;
		};
		uint16_t addr;
	};
	addr = Latch::Addr.s0;

	EMU->SetCHR_ROM8(0x0, CHRbank);
	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRGbank << 1) | (PRG16));
		EMU->SetPRG_ROM16(0xC, (PRGbank << 1) | (PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8, PRGbank);
	if (Mir_A11)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
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

const MapperInfo MapperInfo_BMC_Generic52in1
(
	"BMC-Generic52in1",
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
