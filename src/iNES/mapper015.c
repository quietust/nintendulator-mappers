/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[4];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(0x8 | (x << 1),Mapper.PRG[x]);
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i]);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	u8 PRGbank = (Val & 0x3F) << 1;
	u8 PRGflip = (Val & 0x80) >> 7;
	Mapper.Mirror = Val & 0x40;
	switch (Addr)
	{
	case 0x000:
		PRGbank &= 0x7C;
		Mapper.PRG[0] = PRGbank | 0 ^ PRGflip;
		Mapper.PRG[1] = PRGbank | 1 ^ PRGflip;
		Mapper.PRG[2] = PRGbank | 2 ^ PRGflip;
		Mapper.PRG[3] = PRGbank | 3 ^ PRGflip;
		break;
	case 0x001:
		Mapper.PRG[0] = PRGbank | 0 ^ PRGflip;
		Mapper.PRG[1] = PRGbank | 1 ^ PRGflip;
		Mapper.PRG[2] = 0x7E | 0 ^ PRGflip;
		Mapper.PRG[3] = 0x7F | 1 ^ PRGflip;
		break;
	case 0x002:
		Mapper.PRG[0] = PRGbank ^ PRGflip;
		Mapper.PRG[1] = PRGbank ^ PRGflip;
		Mapper.PRG[2] = PRGbank ^ PRGflip;
		Mapper.PRG[3] = PRGbank ^ PRGflip;
		break;
	case 0x003:
		Mapper.PRG[0] = PRGbank | 0 ^ PRGflip;
		Mapper.PRG[1] = PRGbank | 1 ^ PRGflip;
		Mapper.PRG[2] = PRGbank | 0 ^ PRGflip;
		Mapper.PRG[3] = PRGbank | 1 ^ PRGflip;
		break;
	}
	Sync();
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x8,Write);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			Mapper.PRG[x] = x;
		Mapper.Mirror = 0;
	}
	Sync();
}

static	u8 MapperNum = 15;
CTMapperInfo	MapperInfo_015 =
{
	&MapperNum,
	_T("100-in-1 Contra Function 16"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};