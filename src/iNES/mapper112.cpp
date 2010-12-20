/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Cmd;
uint8 PRG[2], CHR[6];
uint8 Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);

	EMU->SetPRG_ROM16(0xC, -1);

	EMU->SetCHR_ROM2(0, CHR[0] >> 1);
	EMU->SetCHR_ROM2(2, CHR[1] >> 1);

	EMU->SetCHR_ROM1(4, CHR[2]);
	EMU->SetCHR_ROM1(5, CHR[3]);
	EMU->SetCHR_ROM1(6, CHR[4]);
	EMU->SetCHR_ROM1(7, CHR[5]);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Cmd = Val & 0x07;	break;
	case 1:	Mirror = ~Val & 1;	break;
	}
	Sync();
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	switch (Cmd)
		{
		case 0:	PRG[0] = Val;	break;
		case 1:	PRG[1] = Val;	break;
		case 2:	CHR[0] = Val;	break;
		case 3:	CHR[1] = Val;	break;
		case 4:	CHR[2] = Val;	break;
		case 5:	CHR[3] = Val;	break;
		case 6:	CHR[4] = Val;	break;
		case 7:	CHR[5] = Val;	break;
		}
	}
	Sync();
}

void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Mirror = Val & 1;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);

	if (ResetType == RESET_HARD)
	{
		Cmd = 0;
		Mirror = 0;

		PRG[0] = 0x00;
		PRG[1] = 0x01;
		CHR[0] = 0x00;
		CHR[1] = 0x02;
		CHR[2] = 0x04;
		CHR[3] = 0x05;
		CHR[4] = 0x06;
		CHR[5] = 0x07;
	}

	Sync();
}

uint8 MapperNum = 112;
} // namespace

const MapperInfo MapperInfo_112 =
{
	&MapperNum,
	_T("Mapper 112 (Asder)"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};