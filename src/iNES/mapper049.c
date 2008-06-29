/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncMirror();
	if (Mapper.Reg & 1)
		MMC3_SyncPRG(0xF,(Mapper.Reg & 0xC0) >> 2);
	else	EMU->SetPRG_ROM32(0x8,(Mapper.Reg & 0x30) >> 4);
	MMC3_SyncCHR_ROM(0x7F,(Mapper.Reg & 0xC0) << 1);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Reg = Val;
	Sync();
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	MMC3_Reset(ResetType);
	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);
	if (ResetType == RESET_HARD)
	{
		Mapper.Reg = 0;
		Sync();
	}
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 49;
CTMapperInfo	MapperInfo_049 =
{
	&MapperNum,
	_T("1993 Super HiK 4-in-1 (MMC3)"),
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