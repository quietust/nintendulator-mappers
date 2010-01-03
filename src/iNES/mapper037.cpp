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
	switch (Reg)
	{
	case 0:	case 1:	case 2:
		MMC3::SyncPRG(0x7, 0x00);
		break;
	case 3:
		MMC3::SyncPRG(0x7, 0x08);
		break;
	case 4:	case 5:	case 6:
		MMC3::SyncPRG(0xF, 0x10);
		break;
	case 7:
		MMC3::SyncPRG(0x7, 0x18);
		break;
	}
	MMC3::SyncCHR_ROM(0x7F, (Reg & 0x4) << 5);
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
	Reg = Val & 7;
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

u8 MapperNum = 37;
} // namespace

CTMapperInfo	MapperInfo_037 =
{
	&MapperNum,
	_T("Super Mario Bros & Tetris & Nintendo World Cup Soccer (MMC3)"),
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