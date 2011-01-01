/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_N163.h"

namespace
{
uint8 PRG[4], CHR[8], NTab[4];
uint16_n IRQcounter;
FCPURead _Read4;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	for (int i = 0; i < 4; i++)
		EMU->SetPRG_ROM8(8 | (i << 1), PRG[i] & 0x3F);
	for (int i = 0; i < 8; i++)
	{
		if ((CHR[i] < 0xE0) || (PRG[1] & (0x40 << (i >> 2))))
			EMU->SetCHR_ROM1(i, CHR[i]);
		else	EMU->SetCHR_RAM1(i, CHR[i] & 0x1F);
	}

	for (int i = 0; i < 4; i++)
	{
		if (NTab[i] < 0xE0)
		{
			EMU->SetCHR_ROM1(i+0x8, NTab[i]);
			EMU->SetCHR_ROM1(i+0xC, NTab[i]);
		}
		else
		{
			EMU->SetCHR_NT1(i+0x8, NTab[i] & 1);
			EMU->SetCHR_NT1(i+0xC, NTab[i] & 1);
		}
	}
	if (PRG[0] & 0x40)
		EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, NTab[i]);
	offset = N163sound::SaveLoad(mode, offset, data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQcounter.s0 & 0x8000) && ((IRQcounter.s0 & 0x7FFF) != 0x7FFF) && ((++IRQcounter.s0 & 0x7FFF) == 0x7FFF))
		EMU->SetIRQ(0);
}

int	MAPINT	Read4 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return N163sound::Read((Bank << 12) | Addr);
	else	return _Read4(Bank, Addr);
}

int	MAPINT	Read5 (int Bank, int Addr)
{
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		return IRQcounter.b1;
	else	return IRQcounter.b0;
}

void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		N163sound::Write((Bank << 12) | Addr, Val);
	else	_Write4(Bank, Addr, Val);
}

void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		IRQcounter.b1 = Val;
	else	IRQcounter.b0 = Val;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[1] = Val;
	else	CHR[0] = Val;
	Sync();
}

void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[3] = Val;
	else	CHR[2] = Val;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[5] = Val;
	else	CHR[4] = Val;
	Sync();
}

void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[7] = Val;
	else	CHR[6] = Val;
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		NTab[1] = Val;
	else	NTab[0] = Val;
	Sync();
}

void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		NTab[3] = Val;
	else	NTab[2] = Val;
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		PRG[1] = Val;
	else	PRG[0] = Val;
	Sync();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		N163sound::Write((Bank << 12) | Addr, Val);
	else	PRG[2] = Val;
	Sync();
}

int	MAPINT	MapperSnd (int Cycles)
{
	return N163sound::Get(Cycles);
}

void	MAPINT	Load (void)
{
	N163sound::Load();
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, Read4);
	EMU->SetCPUReadHandler(0x5, Read5);

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write4);
	EMU->SetCPUWriteHandler(0x5, Write5);
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
		for (int i = 0; i < 4; i++)
		{
			PRG[i] = 0xFF;
			CHR[i | 0] = 0xFF;
			CHR[i | 4] = 0xFF;
			NTab[i] = 0xFF;
		}
		IRQcounter.s0 = 0;
	}
	N163sound::Reset(ResetType);
	EMU->SetIRQ(1);
	Sync();
}
void	MAPINT	Unload (void)
{
	N163sound::Unload();
}

uint8 MapperNum = 19;
} // namespace

const MapperInfo MapperInfo_019 =
{
	&MapperNum,
	_T("Namcot 106"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};