/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled;
	u16 IRQcounter;
	u16_n IRQlatch;
	u8 PRG;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled);
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter);
	SAVELOAD_WORD(mode,x,data,Mapper.IRQlatch.s0);
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled & 2)
	{
		int mask = (Mapper.IRQenabled & 4) ? 0xFF : 0xFFFF;
		if ((Mapper.IRQcounter & mask) == mask)
		{
			Mapper.IRQcounter = Mapper.IRQlatch.s0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n0 = Val & 0xF;
}

static	void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n1 = Val & 0xF;
}

static	void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n2 = Val & 0xF;
}

static	void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n3 = Val & 0xF;
}

static	void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.IRQenabled = Val & 0x7;
	if (Mapper.IRQenabled & 0x2)
		Mapper.IRQcounter = Mapper.IRQlatch.s0;
	EMU->SetIRQ(1);
}

static	void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Mapper.IRQenabled & 0x1)
		Mapper.IRQenabled |= 0x2;
	else	Mapper.IRQenabled &= ~0x2;
	EMU->SetIRQ(1);
}

static	void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	Mapper.PRG = Val & 0xF;
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	if (ResetType == RESET_HARD)
	{
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter = Mapper.IRQlatch.s0 = 0;
		Mapper.PRG = 0;
	}

	Sync();
}

static	u8 MapperNum = 73;
CTMapperInfo	MapperInfo_073 =
{
	&MapperNum,
	_T("Konami VRC3"),
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
