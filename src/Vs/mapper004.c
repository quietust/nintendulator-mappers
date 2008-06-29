/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_MMC3.h"
#include	"..\Hardware\h_VS.h"

static	void	Sync (void)
{
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
	else	MMC3_SyncMirror();
	MMC3_SyncWRAM();	// assume WRAM is here
	MMC3_SyncPRG(0x3F,0);
	if (ROM->INES_CHRSize)
		MMC3_SyncCHR_ROM(0xFF,0);
	else	MMC3_SyncCHR_RAM(0x07,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	x = VS_SaveLoad(mode,x,data);
	return x;
}

static	void	MAPINT	Load (void)
{
	VS_Load();
	MMC3_Load(Sync);
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	VS_Reset(ResetType);
	MMC3_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
	VS_Unload();
}

static	u8 MapperNum = 4;
CTMapperInfo	MapperInfo_004 =
{
	&MapperNum,
	_T("MMC3/MMC6"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS_CPUCycle,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	VS_Config
};
