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
	EMU->SetCHR_RAM8(0,0);
	if (Latch.Data & 0x20)
	{
		EMU->SetPRG_ROM16(0x8,(Latch.Data & 0x1F));
		EMU->SetPRG_ROM16(0xC,(Latch.Data & 0x1F));
	}
	else	EMU->SetPRG_ROM32(0x8,((Latch.Data & 0x1E) >> 1));
	switch ((Latch.Data & 0xC0) >> 6)
	{
	case 0:	EMU->Mirror_Custom(0,0,0,1);	break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_H();		break;
	case 3:	EMU->Mirror_S1();		break;
	}
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

static	u8 MapperNum = 233;
CTMapperInfo	MapperInfo_233 =
{
	&MapperNum,
	_T("42-in-1"),
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