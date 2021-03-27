/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_N118.h"

namespace
{
void	Sync (void)
{
	N118::SyncPRG();
	EMU->SetCHR_ROM2(0x0, N118::CHR[0] >> 1);
	EMU->SetCHR_ROM2(0x2, N118::CHR[1] >> 1);
	EMU->SetCHR_ROM1(0x4, N118::CHR[2] | 0x40);
	EMU->SetCHR_ROM1(0x5, N118::CHR[3] | 0x40);
	EMU->SetCHR_ROM1(0x6, N118::CHR[4] | 0x40);
	EMU->SetCHR_ROM1(0x7, N118::CHR[5] | 0x40);
}

BOOL	MAPINT	Load (void)
{
	N118::Load(Sync, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	N118::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N118::Unload();
}

uint16_t MapperNum = 88;
} // namespace

const MapperInfo MapperInfo_088
(
	&MapperNum,
	_T("Namco 118 with special CHR banking"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad,
	NULL,
	NULL
);
