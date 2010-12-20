/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 IRQenabled;
uint16_n IRQcounter;
uint8 PRG;
uint8 Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, PRG);
	EMU->SetPRG_ROM32(0x8, 3);
	if (Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		int lastctr = IRQcounter.s0;
		IRQcounter.s0++;
		if ((IRQcounter.s0 & 0x6000) != (lastctr & 0x6000))
		{
			if ((IRQcounter.s0 & 0x6000) == 0x6000)
				EMU->SetIRQ(0);
			else	EMU->SetIRQ(1);
		}
	}
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	PRG = Val & 0x0F;	break;
	case 1:	Mirror = Val & 0x8;	break;
	case 2:	if (Val & 0x02)
			IRQenabled = 1;
		else
		{
			IRQenabled = 0;
			IRQcounter.s0 = 0;
			EMU->SetIRQ(1);
		}				break;
	case 3:					break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0xE; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = Mirror = 0;
		IRQenabled = 0;
		IRQcounter.s0 = 0;
	}

	Sync();
}

uint8 MapperNum = 42;
} // namespace

const MapperInfo MapperInfo_042 =
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