/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
u8 PRG, CHR[8], Mirror;
u8 IRQenabled;
u16_n IRQcounter;

void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	for (x = 0; x < 8; x++)
	{
		if (ROM->INES_CHRSize)
			EMU->SetCHR_ROM1(x, CHR[x]);
		else	EMU->SetCHR_RAM1(x, CHR[x] & 7);
	}
	switch (Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_WORD(mode, x, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_BYTE(mode, x, data, PRG);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled) && (!--IRQcounter.s0))
		EMU->SetIRQ(0);
}

int	ReadEPROM (void)
{
//	EMU->DbgOut("Mapper 16 EPROM read!");
/*
1. start transfer - 00, 40, 60, 20, 00
2. address+mode selection - R/W is MSB, address is lower bits
3. data sync - 40, 60, E0 (and wait for 5 zero bits in a row on reading $xxx0)
4. write data - 00/20/00 for zero, 00/40/60/40/00 for one, 8 bits total
3. data sync - 40, 60, E0 (and wait for 5 zero bits in a row on reading $xxx0)
6. end transfer - 00, 20, 50, 40, 00
*/
	return 0;
}

void	WriteEPROM (int Val)
{
//	EMU->DbgOut("Mapper 16 EPROM write!");
/*
1. start transfer - 00, 40, 60, 20, 00
2. address+mode selection - R/W is MSB, address is lower bits
3. data sync - 40, 60, E0 (and wait for 5 zero bits in a row on reading $xxx0)
5. data sync - 60, E0, wait to read 40?
6. end transfer - 00, 20, 50, 40, 00
wait a bit
*/
}

int	MAPINT	Read (int Bank, int Addr)
{
	if ((Addr & 0xF) == 0)
		return ReadEPROM();
	else	return -1;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xF)
	{
	case 0x0:	CHR[0] = Val;		break;
	case 0x1:	CHR[1] = Val;		break;
	case 0x2:	CHR[2] = Val;		break;
	case 0x3:	CHR[3] = Val;		break;
	case 0x4:	CHR[4] = Val;		break;
	case 0x5:	CHR[5] = Val;		break;
	case 0x6:	CHR[6] = Val;		break;
	case 0x7:	CHR[7] = Val;		break;
	case 0x8:	PRG = Val;		break;
	case 0x9:	Mirror = Val & 0x3;	break;
	case 0xA:	IRQenabled = Val & 1;
			EMU->SetIRQ(1);			break;
	case 0xB:	IRQcounter.b0 = Val;	break;
	case 0xC:	IRQcounter.b1 = Val;	break;
	case 0xD:	WriteEPROM(Val);		break;
	}
	Sync();
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ROM->INES_Flags & 0x02)
	{
		for (x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x, Write);
	}
	else
	{
		for (x = 0x6; x < 0x8; x++)
			EMU->SetCPUReadHandler(x, Read);
		for (x = 0x6; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x, Write);
	}

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		for (x = 0; x < 8; x++)
			CHR[x] = x;
		IRQenabled = 0;
		IRQcounter.s0 = 0;
	}
	Sync();
}

u8 MapperNum = 16;
} // namespace

CTMapperInfo	MapperInfo_016 =
{
	&MapperNum,
	_T("Bandai"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};