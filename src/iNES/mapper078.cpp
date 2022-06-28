/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
enum MIRROR	{ MIRROR_S0S1, MIRROR_HV };
MIRROR MirrMode;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data & 0x7);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM8(0x0, (Latch::Data & 0xF0) >> 4);
	switch (MirrMode)
	{
	case MIRROR_S0S1:
		if (Latch::Data & 0x8)
			EMU->Mirror_S1();
		else	EMU->Mirror_S0();
		break;
	case MIRROR_HV:
		if (Latch::Data & 0x8)
			EMU->Mirror_V();
		else	EMU->Mirror_H();
		break;
	}
}

BOOL	MAPINT	Load (void)
{
	MirrMode = (ROM->INES_Flags & 0x08) ? MIRROR_HV : MIRROR_S0S1;
	if (ROM->INES_Version == 2)
	{
		switch (ROM->INES2_SubMapper)
		{
		case 1:	MirrMode = MIRROR_S0S1;	break;
		case 3:	MirrMode = MIRROR_HV;	break;
		}
	}
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

uint16_t MapperNum = 78;
} // namespace

const MapperInfo MapperInfo_078
(
	&MapperNum,
	_T("Irem 74161/32"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
);
