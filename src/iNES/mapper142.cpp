/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRGcontrol;
	u8 PRG[4];
	u16_n IRQcounter;
	u8 IRQenabled;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
	EMU->SetPRG_ROM8(0x6,Mapper.PRG[3]);
	EMU->SetPRG_ROM8(0xE,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGcontrol);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i]);
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0);
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		if (Mapper.IRQcounter.s0 == 0xFFFF)
		{
			Mapper.IRQenabled = 0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter.s0++;
	}
}

static	void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.IRQcounter.n0 = Val & 0xF;
}

static	void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.IRQcounter.n1 = Val & 0xF;
}

static	void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.IRQcounter.n2 = Val & 0xF;
}

static	void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Mapper.IRQcounter.n3 = Val & 0xF;
}

static	void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.IRQenabled = Val & 0xF;
	EMU->SetIRQ(1);
}

static	void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.PRGcontrol = Val & 0xF;
}

static	void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (Mapper.PRGcontrol)
	{
	case 0x1:	Mapper.PRG[0] = Val & 0xF;	break;
	case 0x2:	Mapper.PRG[1] = Val & 0xF;	break;
	case 0x3:	Mapper.PRG[2] = Val & 0xF;	break;
	case 0x4:	Mapper.PRG[3] = Val & 0xF;	break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			Mapper.PRG[x] = 0;
		Mapper.PRGcontrol = 0;
	}

	Sync();
}

static	u8 MapperNum = 142;
CTMapperInfo	MapperInfo_142 =
{
	&MapperNum,
	_T("SMB2j Pirate (KS 202)"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};