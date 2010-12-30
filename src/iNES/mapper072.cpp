/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper068.cpp $
 * $Id: mapper068.cpp 1077 2010-12-20 04:16:53Z quietust $
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG, CHR, Cmd;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM8(0, CHR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHR);
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Val & 0xC0)
	{
	case 0x00:
		switch (Cmd & 0xC0)
		{
		case 0x40:	CHR = Cmd & 0xF;	break;
		case 0x80:	PRG = Cmd & 0xF;	break;
		}		break;
	case 0x40:	case 0x80:
		Cmd = Val;	break;
	case 0xC0:		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(0x8, Write);

	if (ResetType == RESET_HARD)
		PRG = CHR = Cmd = 0;

	Sync();
}

uint8 MapperNum = 72;
} // namespace

const MapperInfo MapperInfo_072 =
{
	&MapperNum,
	_T("Pinball Quest"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};