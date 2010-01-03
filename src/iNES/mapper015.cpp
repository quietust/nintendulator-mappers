/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRG[4];
u8 Mirror;

void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6, 0);
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(0x8 | (x << 1), PRG[x]);
	EMU->SetCHR_RAM8(0, 0);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	u8 PRGbank = (Val & 0x3F) << 1;
	u8 PRGflip = (Val & 0x80) >> 7;
	Mirror = Val & 0x40;
	switch (Addr)
	{
	case 0x000:
		PRGbank &= 0x7C;
		PRG[0] = PRGbank | 0 ^ PRGflip;
		PRG[1] = PRGbank | 1 ^ PRGflip;
		PRG[2] = PRGbank | 2 ^ PRGflip;
		PRG[3] = PRGbank | 3 ^ PRGflip;
		break;
	case 0x001:
		PRG[0] = PRGbank | 0 ^ PRGflip;
		PRG[1] = PRGbank | 1 ^ PRGflip;
		PRG[2] = 0x7E | 0 ^ PRGflip;
		PRG[3] = 0x7F | 1 ^ PRGflip;
		break;
	case 0x002:
		PRG[0] = PRGbank ^ PRGflip;
		PRG[1] = PRGbank ^ PRGflip;
		PRG[2] = PRGbank ^ PRGflip;
		PRG[3] = PRGbank ^ PRGflip;
		break;
	case 0x003:
		PRG[0] = PRGbank | 0 ^ PRGflip;
		PRG[1] = PRGbank | 1 ^ PRGflip;
		PRG[2] = PRGbank | 0 ^ PRGflip;
		PRG[3] = PRGbank | 1 ^ PRGflip;
		break;
	}
	Sync();
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x8, Write);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			PRG[x] = x;
		Mirror = 0;
	}
	Sync();
}

u8 MapperNum = 15;
} // namespace

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