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
			unsigned PRGsize : 1;
			unsigned Mir_HV  : 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 4;
			unsigned CHRprot : 1;
			unsigned PRGchip : 1;
			unsigned LastBank: 1;
			unsigned         : 9;
		};
		uint16 addr;
	};
	addr = Latch::Addr.s0;

	if (Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (PRGsize) 
		EMU->SetPRG_ROM32(0x8, (PRGchip << 4) | (PRGbank));
	else
	{
		EMU->SetPRG_ROM16(0x8, (PRGchip << 5) | (PRGbank << 1) | (PRG16));
		EMU->SetPRG_ROM16(0xC, (PRGchip << 5) | (PRGbank << 1) | (PRG16));
	}

	if (CHRprot)
	{
		for (int i = 0; i < 8; i++)
		{
			EMU->SetCHR_RAM1(i, i);
			EMU->SetCHR_Ptr1(i, EMU->GetCHR_Ptr1(i), FALSE);
		}
	}
	else
	{	EMU->SetCHR_RAM8(0, 0);
		if (LastBank)
			EMU->SetPRG_ROM16(0xC, (PRGchip << 5) | (PRGbank << 1) |  7);
		else	EMU->SetPRG_ROM16(0xC, (PRGchip << 5) | (PRGbank << 1) & ~7);
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

uint8 MapperNum = 227;
} // namespace

const MapperInfo MapperInfo_227 =
{
	&MapperNum,
	_T("1200-in-1"),
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