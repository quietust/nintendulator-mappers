/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_N118.h"

namespace N118
{
u8 Cmd;
u8 PRG[2];
u8 CHR[6];
FSync Sync;

void	Load (FSync _Sync)
{
	Sync = _Sync;
}
void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0xC;	PRG[1] = 0xD;

		CHR[0] = 0x00;	CHR[1] = 0x02;
		CHR[2] = 0x04;	CHR[3] = 0x05;	CHR[4] = 0x06;	CHR[5] = 0x07;
		Cmd = 0;
	}
	EMU->SetCPUWriteHandler(0x8, CPUWrite89);
	EMU->SetCPUWriteHandler(0x9, CPUWrite89);
	Sync();
}
void	Unload (void)
{
	Sync = NULL;
}

void	SyncPRG (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, 0xF);
}

void	SyncCHR (void)
{
	EMU->SetCHR_ROM2(0, CHR[0] >> 1);
	EMU->SetCHR_ROM2(2, CHR[1] >> 1);
	EMU->SetCHR_ROM1(4, CHR[2]);
	EMU->SetCHR_ROM1(5, CHR[3]);
	EMU->SetCHR_ROM1(6, CHR[4]);
	EMU->SetCHR_ROM1(7, CHR[5]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode, x, data, Cmd);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUWrite89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (Cmd)
		{
		case 0:	CHR[0] = Val & 0x3E;	break;
		case 1:	CHR[1] = Val & 0x3E;	break;
		case 2:	CHR[2] = Val & 0x3F;	break;
		case 3:	CHR[3] = Val & 0x3F;	break;
		case 4:	CHR[4] = Val & 0x3F;	break;
		case 5:	CHR[5] = Val & 0x3F;	break;
		case 6:	PRG[0] = Val & 0xF;	break;
		case 7:	PRG[1] = Val & 0xF;	break;
		}
	else	Cmd = Val & 0x7;
	Sync();
}
} // namespace N118