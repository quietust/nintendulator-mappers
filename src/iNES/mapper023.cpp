/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

#define IRQ_CYCLES 341
namespace
{
uint8 IRQenabled, IRQcounter;
uint8_n IRQlatch;
int16 IRQcycles;
uint8 PRGswap;
uint8 PRG[2];
uint8_n CHR[8];
uint8 Mirror;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM8(0x8, PRGswap ? 0x1E : PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRGswap ? PRG[0] : 0x1E);
	EMU->SetPRG_ROM8(0xE, 0x1F);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i].b0);
	switch (Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch.b0);
	SAVELOAD_WORD(mode, offset, data, IRQcycles);
	SAVELOAD_BYTE(mode, offset, data, PRGswap);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled & 2) && ((IRQenabled & 4) || ((IRQcycles -= 3) < 0)))
	{
		if (!(IRQenabled & 4))
			IRQcycles += IRQ_CYCLES;
		if (IRQcounter == 0xFF)
		{
			IRQcounter = IRQlatch.b0;
			EMU->SetIRQ(0);
		}
		else	IRQcounter++;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	PRG[0] = Val & 0x1F;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	if (Addr & 0x2)
		PRGswap = Val & 0x2;
	else	Mirror = Val & 0x3;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	PRG[1] = Val & 0x1F;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	switch (Addr & 0x3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 1:	CHR[0].n1 = Val & 0xF;	break;
	case 2:	CHR[1].n0 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	switch (Addr & 0x3)
	{
	case 0:	CHR[2].n0 = Val & 0xF;	break;
	case 1:	CHR[2].n1 = Val & 0xF;	break;
	case 2:	CHR[3].n0 = Val & 0xF;	break;
	case 3:	CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	switch (Addr & 0x3)
	{
	case 0:	CHR[4].n0 = Val & 0xF;	break;
	case 1:	CHR[4].n1 = Val & 0xF;	break;
	case 2:	CHR[5].n0 = Val & 0xF;	break;
	case 3:	CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	switch (Addr & 0x3)
	{
	case 0:	CHR[6].n0 = Val & 0xF;	break;
	case 1:	CHR[6].n1 = Val & 0xF;	break;
	case 2:	CHR[7].n0 = Val & 0xF;	break;
	case 3:	CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	Addr = (Addr & 0x003) | ((Addr & 0x00C) >> 2);
	switch (Addr & 0x3)
	{
	case 0:	IRQlatch.n0 = Val & 0xF;	break;
	case 1:	IRQlatch.n1 = Val & 0xF;	break;
	case 2:	IRQenabled = Val & 0x7;
		if (IRQenabled & 0x2)
		{
			IRQcounter = IRQlatch.b0;
			IRQcycles = IRQ_CYCLES;
		}
		EMU->SetIRQ(1);			break;
	case 3:	if (IRQenabled & 0x1)
			IRQenabled |= 0x2;
		else	IRQenabled &= ~0x2;
		EMU->SetIRQ(1);			break;
	}
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		IRQenabled = IRQcounter = IRQlatch.b0 = 0;
		IRQcycles = 0;
		PRGswap = 0;
		PRG[0] = 0;
		PRG[1] = 1;
		for (int i = 0; i < 8; i++)
			CHR[i].b0 = i;
	}

	Sync();
}

uint8 MapperNum = 23;
} // namespace

const MapperInfo MapperInfo_023 =
{
	&MapperNum,
	_T("Konami VRC4 (e/f)"),
	COMPAT_NEARLY,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};