/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_N118.h"
#include	"..\Hardware\h_VS.h"

namespace
{
void	Sync (void)
{
	N118::SyncPRG();
	N118::SyncCHR();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = N118::SaveLoad(mode, offset, data));
	CheckSave(offset = VS::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
		Sync();
	return offset;
}

BOOL	MAPINT	Load (void)
{
	VS::Load();
	N118::Load(Sync, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->Mirror_4S();
	VS::Reset(ResetType);
	N118::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N118::Unload();
	VS::Unload();
}

uint16_t MapperNum = 206;
} // namespace

const MapperInfo MapperInfo_206
(
	&MapperNum,
	_T("Namcot 108"),
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
