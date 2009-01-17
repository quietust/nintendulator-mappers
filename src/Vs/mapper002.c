/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_Latch.h"
#include	"..\Hardware\h_VS.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Latch.Data);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch_SaveLoad_D(mode,x,data);
	x = VS_SaveLoad(mode,x,data);
	return x;
}

static	void	MAPINT	Load (void)
{
	VS_Load();
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	VS_Reset(ResetType);
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
	VS_Unload();
}

static	u8 MapperNum = 2;
CTMapperInfo	MapperInfo_002 =
{
	&MapperNum,
	_T("UNROM"),
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