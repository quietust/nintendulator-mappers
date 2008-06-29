/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

static	void	MAPINT	Load (void)
{
	VS_Load();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	VS_Reset(ResetType);
	iNES_SetMirroring();

	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,0);
}
static	void	MAPINT	Unload (void)
{
	VS_Unload();
}

static	u8 MapperNum = 0;
CTMapperInfo	MapperInfo_000 =
{
	&MapperNum,
	_T("NROM"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS_CPUCycle,
	NULL,
	VS_SaveLoad,
	NULL,
	VS_Config
};