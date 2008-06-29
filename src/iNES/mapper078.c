/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Latch.Data & 0x7);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM8(0,(Latch.Data & 0xF0) >> 4);
	if (Latch.Data & 0x8)
		((ROM->INES_Flags & 0x08) ? EMU->Mirror_V : EMU->Mirror_S1)();
	else	((ROM->INES_Flags & 0x08) ? EMU->Mirror_H : EMU->Mirror_S0)();
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 78;
CTMapperInfo	MapperInfo_078 =
{
	&MapperNum,
	_T("Irem 74161/32"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};