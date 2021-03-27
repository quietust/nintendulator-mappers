/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_Latch.h"
#include	"..\Hardware\h_VS.h"

namespace
{
void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, Latch::Data);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_RAM8(0x0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = Latch::SaveLoad_D(mode, offset, data));
	CheckSave(offset = VS::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
		Sync();
	return offset;
}

BOOL	MAPINT	Load (void)
{
	VS::Load();
	Latch::Load(Sync, FALSE, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	VS::Reset(ResetType);
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
	VS::Unload();
}

uint16_t MapperNum = 2;
} // namespace

const MapperInfo MapperInfo_002
(
	&MapperNum,
	_T("UNROM"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
);
