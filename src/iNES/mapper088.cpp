/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_N118.h"

namespace
{
void	Sync (void)
{
	N118::SyncPRG();
	EMU->SetCHR_ROM2(0, N118::CHR[0] >> 1);
	EMU->SetCHR_ROM2(2, N118::CHR[1] >> 1);
	EMU->SetCHR_ROM1(4, N118::CHR[2] | 0x40);
	EMU->SetCHR_ROM1(5, N118::CHR[3] | 0x40);
	EMU->SetCHR_ROM1(6, N118::CHR[4] | 0x40);
	EMU->SetCHR_ROM1(7, N118::CHR[5] | 0x40);
}

void	MAPINT	Load (void)
{
	N118::Load(Sync);
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

u8 MapperNum = 88;
} // namespace

CTMapperInfo	MapperInfo_088 =
{
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
};