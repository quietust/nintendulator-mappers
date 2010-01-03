/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
u8 Reg;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0xF, Reg << 4);
	MMC3::SyncCHR_ROM(0x7F, Reg << 7);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3::SaveLoad(mode, x, data);
	SAVELOAD_BYTE(mode, x, data, Reg);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Reg = Val & 1;
	Sync();
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
	{
		Reg = 0;
		Sync();
	}
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

u8 MapperNum = 47;
} // namespace

CTMapperInfo	MapperInfo_047 =
{
	&MapperNum,
	_T("Super Spike & Nintendo World Cup Soccer (MMC3)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
};