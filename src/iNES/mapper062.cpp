/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
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
			unsigned CHRhi   : 5;
			unsigned PRGsize : 1;
			unsigned PRGchip : 1;
			unsigned Mir_A11 : 1;
			unsigned PRG     : 6;
			unsigned         : 2;
			unsigned CHRlo   : 2;
			unsigned         : 6;
		};
		struct
		{
			unsigned addr    :16;
			unsigned data    : 8;
		};
	};
	addr = Latch::Addr.s0;
	data = Latch::Data;

	EMU->SetCHR_ROM8(0x0, (CHRhi << 2) | CHRlo);

	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRGchip << 6) | PRG);
		EMU->SetPRG_ROM16(0xC, (PRGchip << 6) | PRG);
	}
	else	EMU->SetPRG_ROM32(0x8, ((PRGchip << 6) | PRG) >> 1);

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

const MapperInfo MapperInfo_062
(
	62,
	_T("700-in-1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_AD,
	NULL,
	NULL
);
