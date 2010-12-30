/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper091.cpp $
 * $Id: mapper091.cpp 1076 2010-12-20 03:59:22Z quietust $
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 PRG;
uint8 CHR[3];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG);
	EMU->SetPRG_ROM8(0xA, -3);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM4(0, CHR[0] >> 2);
	EMU->SetCHR_ROM2(4, CHR[1] >> 1);
	EMU->SetCHR_ROM2(6, CHR[2] >> 1);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[0] = Val;	break;
	case 1:	CHR[1] = Val;	break;
	case 2:	CHR[2] = Val;	break;
	case 3:	PRG = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		CHR[0] = CHR[1] = CHR[2] = 0;

	}
	Sync();
}

uint8 MapperNum = 193;
} // namespace

const MapperInfo MapperInfo_193 =
{
	&MapperNum,
	_T("Fighting Hero"),
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