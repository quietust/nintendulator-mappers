/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG;
uint8_t CHR[4];
uint8_t Mirror;
uint16_n IRQcounter;
uint8_t IRQenabled;
uint8_t IRQpos;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);

	EMU->SetCHR_ROM2(0x0, CHR[0]);
	EMU->SetCHR_ROM2(0x2, CHR[1]);
	EMU->SetCHR_ROM2(0x4, CHR[2]);
	EMU->SetCHR_ROM2(0x6, CHR[3]);

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
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQpos);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		if (!IRQcounter.s0)
		{
			IRQenabled = 0;
			IRQcounter.s0 = 0xFFFF;
			EMU->SetIRQ(0);
		}
		else	IRQcounter.s0--;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[0] = Val;
		Sync();
	}
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[1] = Val;
		Sync();
	}
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[2] = Val;
		Sync();
	}
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[3] = Val;
		Sync();
	}
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		if (IRQpos)
			IRQcounter.b0 = Val;
		else	IRQcounter.b1 = Val;
		IRQpos ^= 1;
	}
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		IRQpos = 0;
		IRQenabled = (Val & 0x10) >> 4;
		EMU->SetIRQ(1);
	}
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		Mirror = Val;
		Sync();
	}
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		PRG = Val;
		Sync();
	}
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
		PRG = 0;
		CHR[0] = 0;	CHR[1] = 0;	CHR[2] = 0;	CHR[3] = 0;
		Mirror = 0;
		IRQcounter.s0 = 0;
		IRQenabled = IRQpos = 0;
	}

	Sync();
}

uint16_t MapperNum = 67;
} // namespace

const MapperInfo MapperInfo_067
(
	&MapperNum,
	_T("Sunsoft Mapper #3"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
