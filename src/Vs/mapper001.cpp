/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_MMC1.h"
#include	"..\Hardware\h_VS.h"

static	void	Sync (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	if (ROM->INES_CHRSize)
		MMC1_SyncCHR_ROM(0x1F,0);
	else	MMC1_SyncCHR_RAM(0x01,0);
	MMC1_SyncWRAM();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC1_SaveLoad(mode,x,data);
	x = VS_SaveLoad(mode,x,data);
	return x;
}
static	void	MAPINT	Load (void)
{
	VS_Load();
	MMC1_Load(Sync);
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	VS_Reset(ResetType);
	MMC1_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC1_Unload();
	VS_Unload();
}

static	u8 MapperNum = 1;
CTMapperInfo	MapperInfo_001 =
{
	&MapperNum,
	_T("MMC1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS_CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS_Config
};