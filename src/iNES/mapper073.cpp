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
uint16 IRQcounter;
uint16_n IRQlatch;
uint8 PRG;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_WORD(mode, offset, data, IRQcounter);
	SAVELOAD_WORD(mode, offset, data, IRQlatch.s0);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled & 2)
	{
		int mask = (IRQenabled & 4) ? 0xFF : 0xFFFF;
		if ((IRQcounter & mask) == mask)
		{
			IRQcounter = IRQlatch.s0;
			EMU->SetIRQ(0);
		}
		else	IRQcounter++;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	IRQlatch.n0 = Val & 0xF;
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	IRQlatch.n1 = Val & 0xF;
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	IRQlatch.n2 = Val & 0xF;
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	IRQlatch.n3 = Val & 0xF;
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	IRQenabled = Val & 0x7;
	if (IRQenabled & 0x2)
		IRQcounter = IRQlatch.s0;
	EMU->SetIRQ(1);
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (IRQenabled & 0x1)
		IRQenabled |= 0x2;
	else	IRQenabled &= ~0x2;
	EMU->SetIRQ(1);
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	PRG = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		IRQenabled = 0;
		IRQcounter = IRQlatch.s0 = 0;
		PRG = 0;
	}

	Sync();
}

uint8 MapperNum = 73;
} // namespace

const MapperInfo MapperInfo_073 =
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