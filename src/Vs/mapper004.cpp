/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_MMC3.h"
#include	"..\Hardware\h_VS.h"

namespace
{
void	Sync (void)
{
	MMC3::SyncPRG(0x3F, 0);
	if (ROM->INES_CHRSize)
		MMC3::SyncCHR_ROM(0xFF, 0);
	else	MMC3::SyncCHR_RAM(0x07, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	CheckSave(offset = VS::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
		Sync();
	return offset;
}

BOOL	MAPINT	Load (void)
{
	VS::Load();
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->Mirror_4S();
	VS::Reset(ResetType);
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
	VS::Unload();
}
} // namespace

const MapperInfo MapperInfo_004
(
	4,
	_T("MMC3/MMC6"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	VS::Config
);
