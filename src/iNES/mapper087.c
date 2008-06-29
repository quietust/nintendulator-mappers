/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Latch;
}	Mapper;

static	void	Sync (void)
{
	const unsigned char lut[4] = {0x00,0x02,0x01,0x03};
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,lut[Mapper.Latch & 0x3]);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Latch = Val;
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	if (ResetType == RESET_HARD)
		Mapper.Latch = 0;

	Sync();
}

static	u8 MapperNum = 87;
CTMapperInfo	MapperInfo_087 =
{
	&MapperNum,
	_T("Jaleco 74138/7474"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};