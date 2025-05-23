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
			unsigned Mir_A11 : 1;
			unsigned PRGsizeL: 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 7;
			unsigned         : 1;
			unsigned PRGhigh : 1;
			unsigned         : 4;
		};
		uint16_t addr;
	};
	addr = Latch::Addr.s0;

	EMU->SetCHR_RAM8(0x0, 0);
	
	if ((PRGbank & 0x60) == 0x60)
		for (int i = 0x8; i < 0x10; i++)
			EMU->SetPRG_OB4(i);
	if (PRGsizeL)
		EMU->SetPRG_ROM32(0x8, PRGbank);
	else
	{
		EMU->SetPRG_ROM16(0x8, (PRGbank << 1) | PRG16);
		EMU->SetPRG_ROM16(0xC, (PRGbank << 1) | PRG16);
	}
	if (PRGhigh)
		EMU->SetPRG_ROM8(0xE, ((PRGbank & 0x1F) << 2) | ((PRGsizeL ? 1 : PRG16) << 1) | 1);
	if (Mir_A11)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();

	if (PRGbank == 0) 
	{	// hack
		EMU->GetPRG_Ptr4(0xF)[0x02A7] = 0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x02A8] = 0xF2;
		EMU->GetPRG_Ptr4(0xF)[0x028A] = 0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x028B] = 0xF2;
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

const MapperInfo MapperInfo_063
(
	63,
	_T("Hello Kitty 255 in 1"),
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
