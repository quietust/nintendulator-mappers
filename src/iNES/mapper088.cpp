/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

typedef	struct	N118
{
	u8 Cmd;
	u8 PRG[2];
	u8 CHR[6];
	FSync Sync;
}	TN118, *PN118;
static	TN118	N118;

static	void	MAPINT	N118_CPUWrite89 (int Bank, int Addr, int Val);

static	void	N118_Load (FSync Sync)
{
	N118.Sync = Sync;
}
static	void	N118_Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		N118.PRG[0] = 0xC;	N118.PRG[1] = 0xD;

		N118.CHR[0] = 0x00;	N118.CHR[1] = 0x02;
		N118.CHR[2] = 0x04;	N118.CHR[3] = 0x05;	N118.CHR[4] = 0x06;	N118.CHR[5] = 0x07;
		N118.Cmd = 0;
	}
	EMU->SetCPUWriteHandler(0x8,N118_CPUWrite89);
	EMU->SetCPUWriteHandler(0x9,N118_CPUWrite89);
	N118.Sync();
}
static	void	N118_Unload (void)
{
	N118.Sync = NULL;
}

static	void	N118_SyncPRG (void)
{
	EMU->SetPRG_ROM8(0x8,N118.PRG[0]);
	EMU->SetPRG_ROM8(0xA,N118.PRG[1]);
	EMU->SetPRG_ROM16(0xC,0xF);
}

static	void	N118_SyncCHR (void)
{
	EMU->SetCHR_ROM2(0,N118.CHR[0]);
	EMU->SetCHR_ROM2(2,N118.CHR[1]);
	EMU->SetCHR_ROM1(4,N118.CHR[2] | 0x40);
	EMU->SetCHR_ROM1(5,N118.CHR[3] | 0x40);
	EMU->SetCHR_ROM1(6,N118.CHR[4] | 0x40);
	EMU->SetCHR_ROM1(7,N118.CHR[5] | 0x40);
}

static	int	MAPINT	N118_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,N118.Cmd);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,N118.PRG[i]);
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode,x,data,N118.CHR[i]);
	if (mode == STATE_LOAD)
		N118.Sync();
	return x;
}

static	void	MAPINT	N118_CPUWrite89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (N118.Cmd)
		{
		case 0:	N118.CHR[0] = (Val & 0x3E) >> 1;break;
		case 1:	N118.CHR[1] = (Val & 0x3E) >> 1;break;
		case 2:	N118.CHR[2] = Val & 0x3F;	break;
		case 3:	N118.CHR[3] = Val & 0x3F;	break;
		case 4:	N118.CHR[4] = Val & 0x3F;	break;
		case 5:	N118.CHR[5] = Val & 0x3F;	break;
		case 6:	N118.PRG[0] = Val & 0xF;	break;
		case 7:	N118.PRG[1] = Val & 0xF;	break;
		}
	else	N118.Cmd = Val & 0x7;
	N118.Sync();
}

static	void	Sync (void)
{
	N118_SyncPRG();
	N118_SyncCHR();
}

static	void	MAPINT	Load (void)
{
	N118_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	N118_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	N118_Unload();
}

static	u8 MapperNum = 88;
CTMapperInfo	MapperInfo_088 =
{
	&MapperNum,
	_T("Namco 118 with special CHR banking"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	N118_SaveLoad,
	NULL,
	NULL
};