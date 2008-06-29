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
	u8 Game;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncMirror();
	MMC3_SyncPRG((Mapper.Game == 6) ? 0x1F : 0x0F,Mapper.Game << 4);
	MMC3_SyncCHR_ROM((Mapper.Game == 6) ? 0xFF : 0x7F,Mapper.Game << 7);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Game);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	MMC3_CPUWriteAB(Bank,Addr,Val);	break;
	case 1:	Mapper.Game = Val & 0x07;
		if (Mapper.Game == 7)
			Mapper.Game = 6;
		Sync();				break;
	}
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ResetType == RESET_HARD)
		Mapper.Game = 0;
	MMC3_Reset(ResetType);
	for (x = 0xA; x < 0xC; x++)
		EMU->SetCPUWriteHandler(x,Write);	/* need to override writes to $A001 */
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 44;
CTMapperInfo	MapperInfo_044 =
{
	&MapperNum,
	_T("Super HiK 7 in 1 (MMC3)"),
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