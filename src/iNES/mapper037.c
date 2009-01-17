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
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncMirror();
	switch (Mapper.Reg)
	{
	case 0:	case 1:	case 2:
		MMC3_SyncPRG(0x7,0x00);
		break;
	case 3:
		MMC3_SyncPRG(0x7,0x08);
		break;
	case 4:	case 5:	case 6:
		MMC3_SyncPRG(0xF,0x10);
		break;
	case 7:
		MMC3_SyncPRG(0x7,0x18);
		break;
	}
	MMC3_SyncCHR_ROM(0x7F,(Mapper.Reg & 0x4) << 5);
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
	Mapper.Reg = Val & 7;
	Sync();
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

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

static	u8 MapperNum = 37;
CTMapperInfo	MapperInfo_037 =
{
	&MapperNum,
	_T("Super Mario Bros & Tetris & Nintendo World Cup Soccer (MMC3)"),
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