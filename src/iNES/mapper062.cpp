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
			unsigned CHRhi   : 5;
			unsigned PRGsize : 1;
			unsigned PRGchip : 1;
			unsigned Mir_HV  : 1;
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

	EMU->SetCHR_ROM8(0, (CHRhi << 2) | CHRlo);

	if (PRGsize)
	{
		EMU->SetPRG_ROM16(0x8, (PRGchip << 6) | PRG);
		EMU->SetPRG_ROM16(0xC, (PRGchip << 6) | PRG);
	}
	else	EMU->SetPRG_ROM32(0x8, ((PRGchip << 6) | PRG) >> 1);

	if (Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE);
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

uint8 MapperNum = 62;
} // namespace

const MapperInfo MapperInfo_062 =
{
	&MapperNum,
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
};