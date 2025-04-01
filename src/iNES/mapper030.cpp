/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
bool WroteFlash;
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data & 0x1F);
	EMU->SetPRG_ROM16(0xC, 0x1F);
	EMU->SetCHR_RAM8(0x0, (Latch::Data & 0x60) >> 5);
	if ((ROM->INES_Flags & 0x09) == 0x08)
	{
		if (Latch::Data & 0x80)
			EMU->Mirror_S1();
		else	EMU->Mirror_S0();
	}
}

void	MAPINT	WriteFlash (int Bank, int Addr, int Val)
{
	if (!WroteFlash)
		EMU->DbgOut(_T("ERROR: game attempted to write to Flash ROM!"));
	WroteFlash = true;
}

BOOL	MAPINT	Load (void)
{
	if (ROM->INES_Flags & 0x02)
		Latch::Load(Sync, FALSE, TRUE);
	else
		Latch::Load(Sync, TRUE, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	WroteFlash = false;
	iNES_SetMirroring();
	Latch::Reset(ResetType);
	if (ROM->INES_Flags & 0x02)
	{
		EMU->SetCPUWriteHandler(0x8, WriteFlash);
		EMU->SetCPUWriteHandler(0x9, WriteFlash);
		EMU->SetCPUWriteHandler(0xA, WriteFlash);
		EMU->SetCPUWriteHandler(0xB, WriteFlash);
	}
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}
} // namespace

const MapperInfo MapperInfo_002
(
	30,
	_T("UNROM 512"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
);
