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
			unsigned PRGbank : 5;
			unsigned         : 3;
			unsigned PRGchip : 2;
			unsigned Mir_S0  : 1;	/* S0 if 1, H/V if 0 */
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned Mir_A11 : 1;	/* A11 if 1, A10 if 0 */
			unsigned         : 2;
		};
		uint16_t addr;
	};
	uint8_t openbus = 0;
	addr = Latch::Addr.s0;
	if (ROM->INES_PRGSize == 64)	/* 1MB, 100-in-1 */
	{
		switch (PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	openbus = 1;	break;
		case 3:	openbus = 1;	break;
		}
	}
	else if (ROM->INES_PRGSize == 128)	/* 2MB, 150-in-1 */
	{
		switch (PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	PRGchip = 1;
					break;
		case 3:	openbus = 1;	break;
		}
	}					/* else 260-in-1 */
	if (openbus)
	{
		for (openbus = 0x8; openbus < 0x10; openbus++)
			EMU->SetPRG_OB4(openbus);
	}
	else
	{
		if (PRGsize)
		{
			EMU->SetPRG_ROM16(0x8, (PRGchip << 6) | (PRGbank << 1) | PRG16);
			EMU->SetPRG_ROM16(0xC, (PRGchip << 6) | (PRGbank << 1) | PRG16);
		}
		else	EMU->SetPRG_ROM32(0x8, (PRGchip << 5) | PRGbank);
	}
	EMU->SetCHR_RAM8(0x0, 0);
	if (Mir_S0)
		EMU->Mirror_S0();
	else if (Mir_A11)
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

const MapperInfo MapperInfo_235
(
	235,
	_T("Golden Game 150-in-1"),
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
