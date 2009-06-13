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
	u16_n IRQcounter;
	u8 PRG;
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,Mapper.PRG);
	EMU->SetPRG_ROM32(0x8,3);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled);
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0);
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		int lastctr = Mapper.IRQcounter.s0;
		Mapper.IRQcounter.s0++;
		if ((Mapper.IRQcounter.s0 & 0x6000) != (lastctr & 0x6000))
		{
			if ((Mapper.IRQcounter.s0 & 0x6000) == 0x6000)
				EMU->SetIRQ(0);
			else	EMU->SetIRQ(1);
		}
	}
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	Mapper.PRG = Val & 0x0F;	break;
	case 1:	Mapper.Mirror = Val & 0x8;	break;
	case 2:	if (Val & 0x02)
			Mapper.IRQenabled = 1;
		else
		{
			Mapper.IRQenabled = 0;
			Mapper.IRQcounter.s0 = 0;
			EMU->SetIRQ(1);
		}				break;
	case 3:					break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0xE; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = Mapper.Mirror = 0;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter.s0 = 0;
	}

	Sync();
}

static	u8 MapperNum = 42;
CTMapperInfo	MapperInfo_042 =
{
	&MapperNum,
	_T("Mario Baby"),
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