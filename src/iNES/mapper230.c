/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	struct
{
	u8 Mode;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mode)
	{
		if (Latch.Data & 0x20)
		{
			EMU->SetPRG_ROM16(0x8,(Latch.Data & 0x1F) + 0x8);
			EMU->SetPRG_ROM16(0xC,(Latch.Data & 0x1F) + 0x8);
		}
		else	EMU->SetPRG_ROM32(0x8,((Latch.Data & 0x1E) >> 1) + 0x4);
		if (Latch.Data & 0x40)
			EMU->Mirror_V();
		else	EMU->Mirror_H();
	}
	else
	{
		EMU->SetPRG_ROM16(0x8,Latch.Data & 0x7);
		EMU->SetPRG_ROM16(0xC,0x7);
		EMU->Mirror_V();
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch_SaveLoad_D(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Mapper.Mode = 0;
	else	Mapper.Mode ^= 1;

	Latch_Reset(ResetType);
}

static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 230;
CTMapperInfo	MapperInfo_230 =
{
	&MapperNum,
	_T("22-in-1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};