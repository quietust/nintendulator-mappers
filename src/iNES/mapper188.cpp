/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
void	Sync (void)
{
	if (Latch::Data & 0x10)
		EMU->SetPRG_ROM16(0x8, Latch::Data & 0x7);
	else	EMU->SetPRG_ROM16(0x8, Latch::Data | 0x8);
	EMU->SetPRG_ROM16(0xC, 0x7);
	EMU->SetCHR_RAM8(0x0, 0);
}

int	MAPINT	Read67 (int Bank, int Addr)
{
	return 3;
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	Latch::Reset(ResetType);
	EMU->SetCPUReadHandler(0x6, Read67);
	EMU->SetCPUReadHandler(0x7, Read67);
	// this is debug-safe
	EMU->SetCPUReadHandlerDebug(0x6, Read67);
	EMU->SetCPUReadHandlerDebug(0x7, Read67);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint16_t MapperNum = 188;
} // namespace

const MapperInfo MapperInfo_188
(
	&MapperNum,
	_T("Mapper 188"),
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
