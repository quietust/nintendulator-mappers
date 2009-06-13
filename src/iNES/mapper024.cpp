/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_VRC6.h"

#define IRQ_CYCLES 341
namespace
{
u8 IRQenabled, IRQcounter, IRQlatch;
s16 IRQcycles;
u8 PRG[2], CHR[8];
u8 Mirror;
u8 SwapAddr[4];

void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM16(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xC, PRG[1]);
	EMU->SetPRG_ROM8(0xE, -1);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x, CHR[x]);
	switch ((Mirror >> 2) & 3)
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
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	x = VRC6sound::SaveLoad(mode, x, data);
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
	switch (SwapAddr[Addr & 3])
	{
	case 0:	PRG[0] = Val;
		Sync();			break;
	case 1:				break;
	case 2:				break;
	case 3:				break;
	}
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	VRC6sound::Write(0x9000 | SwapAddr[Addr & 3], Val);
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	VRC6sound::Write(0xA000 | SwapAddr[Addr & 3], Val);
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (SwapAddr[Addr & 3])
	{
	case 0:	case 1:	case 2:
		VRC6sound::Write(0xB000 | SwapAddr[Addr & 3], Val);
					break;
	case 3:	Mirror = Val;
		Sync();			break;
	}
	
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (SwapAddr[Addr & 3])
	{
	case 0:	PRG[1] = Val;
		Sync();			break;
	case 1:				break;
	case 2:				break;
	case 3:				break;
	}
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	CHR[SwapAddr[Addr & 3]] = Val;
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	CHR[4 | SwapAddr[Addr & 3]] = Val;
	Sync();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (SwapAddr[Addr & 3])
	{
	case 0:	IRQlatch = Val;	break;
	case 1:	IRQenabled = Val & 0x7;
		if (IRQenabled & 0x2)
		{
			IRQcounter = IRQlatch;
			IRQcycles = IRQ_CYCLES;
		}
		EMU->SetIRQ(1);
		break;
	case 2:	if (IRQenabled & 0x1)
			IRQenabled |= 0x2;
		else	IRQenabled &= ~0x2;
		EMU->SetIRQ(1);		break;
	case 3:				break;
	}
}

int	MAPINT	MapperSnd (int Cycles)
{
	return VRC6sound::Get(Cycles);
}

void	MAPINT	Load_024 (void)
{
	VRC6sound::Load();
	SwapAddr[0] = 0;
	SwapAddr[1] = 1;
	SwapAddr[2] = 2;
	SwapAddr[3] = 3;
	iNES_SetSRAM();
}
void	MAPINT	Load_026 (void)
{
	VRC6sound::Load();
	SwapAddr[0] = 0;
	SwapAddr[1] = 2;
	SwapAddr[2] = 1;
	SwapAddr[3] = 3;
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
		PRG[0] = 0;
		PRG[1] = 0xFE;
		for (x = 0; x < 8; x++)
			CHR[x] = x;
		IRQenabled = IRQcounter = IRQlatch = 0;
		IRQcycles = 0;
	}

	VRC6sound::Reset(ResetType);
	Sync();
}
void	MAPINT	Unload (void)
{
	VRC6sound::Unload();
}

u8 MapperNum = 24;
u8 MapperNum2 = 26;
} // namespace

CTMapperInfo	MapperInfo_024 =
{
	&MapperNum,
	_T("Konami VRC6 (A0/A1)"),
	COMPAT_FULL,
	Load_024,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};

CTMapperInfo	MapperInfo_026 =
{
	&MapperNum2,
	_T("Konami VRC6 (A1/A0)"),
	COMPAT_FULL,
	Load_026,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};
