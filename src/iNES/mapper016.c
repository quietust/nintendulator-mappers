/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG, CHR[8], Mirror;
	u8 IRQenabled;
	u16_n IRQcounter;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,-1);
	for (x = 0; x < 8; x++)
	{
		if (ROM->INES_CHRSize)
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
		else	EMU->SetCHR_RAM1(x,Mapper.CHR[x] & 7);
	}
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0);
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled);
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i]);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled) && (!--Mapper.IRQcounter.s0))
		EMU->SetIRQ(0);
}

static	int	ReadEPROM (void)
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

static	void	WriteEPROM (int Val)
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

static	int	MAPINT	Read (int Bank, int Addr)
{
	if ((Addr & 0xF) == 0)
		return ReadEPROM();
	else	return -1;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xF)
	{
	case 0x0:	Mapper.CHR[0] = Val;		break;
	case 0x1:	Mapper.CHR[1] = Val;		break;
	case 0x2:	Mapper.CHR[2] = Val;		break;
	case 0x3:	Mapper.CHR[3] = Val;		break;
	case 0x4:	Mapper.CHR[4] = Val;		break;
	case 0x5:	Mapper.CHR[5] = Val;		break;
	case 0x6:	Mapper.CHR[6] = Val;		break;
	case 0x7:	Mapper.CHR[7] = Val;		break;
	case 0x8:	Mapper.PRG = Val;		break;
	case 0x9:	Mapper.Mirror = Val & 0x3;	break;
	case 0xA:	Mapper.IRQenabled = Val & 1;
			EMU->SetIRQ(1);			break;
	case 0xB:	Mapper.IRQcounter.b0 = Val;	break;
	case 0xC:	Mapper.IRQcounter.b1 = Val;	break;
	case 0xD:	WriteEPROM(Val);		break;
	}
	Sync();
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ROM->INES_Flags & 0x02)
	{
		for (x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x,Write);
	}
	else
	{
		for (x = 0x6; x < 0x8; x++)
			EMU->SetCPUReadHandler(x,Read);
		for (x = 0x6; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x,Write);
	}

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = 0;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = x;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter.s0 = 0;
	}
	Sync();
}

static	u8 MapperNum = 16;
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
