/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 Latch;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.Latch);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0x3F,0);
	MMC3_SyncMirror();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if (Addr == 0x120)
	{
		Mapper.Latch = Val >> 4;
		Sync();
	}
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write4);

	if (ResetType == RESET_HARD)
		Mapper.Latch = 0;

	MMC3_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 189;
CTMapperInfo	MapperInfo_189 =
{
	&MapperNum,
	_T("Mapper 189 (Thunder Warrior)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};