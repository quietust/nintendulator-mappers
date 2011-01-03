/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
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
	N118::SyncCHR();
	EMU->SetCHR_NT1(0x8, N118::CHR[0] >> 5);
	EMU->SetCHR_NT1(0x9, N118::CHR[0] >> 5);
	EMU->SetCHR_NT1(0xA, N118::CHR[1] >> 5);
	EMU->SetCHR_NT1(0xB, N118::CHR[1] >> 5);
	EMU->SetCHR_NT1(0xC, N118::CHR[2] >> 5);
	EMU->SetCHR_NT1(0xD, N118::CHR[3] >> 5);
	EMU->SetCHR_NT1(0xE, N118::CHR[4] >> 5);
	EMU->SetCHR_NT1(0xF, N118::CHR[5] >> 5);
}

void	MAPINT	Load (void)
{
	N118::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	N118::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N118::Unload();
}

uint8 MapperNum = 95;
} // namespace

const MapperInfo MapperInfo_095 =
{
	&MapperNum,
	_T("Namco 118, CHR-controlled mirroring"),
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