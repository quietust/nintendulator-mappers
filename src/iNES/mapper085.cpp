/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_VRC7.h"

#define IRQ_CYCLES 341
namespace
{
u8 IRQenabled, IRQcounter, IRQlatch;
s16 IRQcycles;
u8 PRG[3], CHR[8], Misc;

void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, -1);
	if (Misc & 0x80)
		EMU->SetPRG_RAM8(0x6, 0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
	if (ROM->INES_CHRSize)
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x, CHR[x]);
	else	for (x = 0; x < 8; x++)
			EMU->SetCHR_RAM1(x, CHR[x] & 7);
	switch (Misc & 0x3)
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
	SAVELOAD_BYTE(mode, x, data, IRQenabled);
	SAVELOAD_BYTE(mode, x, data, IRQcounter);
	SAVELOAD_BYTE(mode, x, data, IRQlatch);
	SAVELOAD_WORD(mode, x, data, IRQcycles);
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, Misc);
	x = VRC7sound::SaveLoad(mode, x, data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled & 2) && ((IRQenabled & 4) || ((IRQcycles -= 3) < 0)))
	{
		if (!(IRQenabled & 4))
			IRQcycles += IRQ_CYCLES;
		if (IRQcounter == 0xFF)
		{
			IRQcounter = IRQlatch;
			EMU->SetIRQ(0);
		}
		else	IRQcounter++;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		PRG[1] = Val;
	else	PRG[0] = Val;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		VRC7sound::Write((Bank << 12) | Addr, Val);
	else
	{
		PRG[2] = Val;
		Sync();
	}
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		CHR[1] = Val;
	else	CHR[0] = Val;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		CHR[3] = Val;
	else	CHR[2] = Val;
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		CHR[5] = Val;
	else	CHR[4] = Val;
	Sync();
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		CHR[7] = Val;
	else	CHR[6] = Val;
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
		IRQlatch = Val;
	else
	{
		Misc = Val;
		Sync();
	}
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x18)
	{
		if (IRQenabled & 0x1)
			IRQenabled |= 0x2;
		else	IRQenabled &= ~0x2;
	}
	else
	{
		IRQenabled = Val & 0x7;
		if (IRQenabled & 0x2)
		{
			IRQcounter = IRQlatch;
			IRQcycles = IRQ_CYCLES;
		}
	}
	EMU->SetIRQ(1);
}

int	MAPINT	MapperSnd (int Cycles)
{
	return VRC7sound::Get(Cycles);
}

void	MAPINT	Load (void)
{
	VRC7sound::Load();
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

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
		IRQenabled = IRQcounter = IRQlatch = 0;
		IRQcycles = 0;
		PRG[0] = 0x00;	PRG[0] = 0x01;	PRG[2] = 0xFE;
		for (x = 0; x < 8; x++)	CHR[x] = x;
		Misc = 0;
	}

	VRC7sound::Reset(ResetType);
	Sync();
}
void	MAPINT	Unload (void)
{
	VRC7sound::Unload();
}

u8 MapperNum = 85;
} // namespace

const MapperInfo MapperInfo_085 =
{
	&MapperNum,
	_T("Konami VRC7"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};