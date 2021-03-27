/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t IRQenabled;
uint16_n IRQcounter;
uint8_t PRG;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, 6);
	EMU->SetPRG_ROM8(0x8, 4);
	EMU->SetPRG_ROM8(0xA, 5);
	EMU->SetPRG_ROM8(0xC, PRG);
	EMU->SetPRG_ROM8(0xE, 7);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0x0, 0);
	else	EMU->SetCHR_RAM8(0x0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, PRG);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		IRQcounter.s0++;
		if (IRQcounter.s0 & 4096)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	IRQenabled = 0;
	IRQcounter.s0 = 0;
	EMU->SetIRQ(1);
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	IRQenabled = 1;
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	PRG = Val & 7;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);

	if (ResetType == RESET_HARD)
	{
		IRQenabled = 0;
		IRQcounter.s0 = 0;
		PRG = 0;
	}

	Sync();
}

uint16_t MapperNum = 40;
} // namespace

const MapperInfo MapperInfo_040
(
	&MapperNum,
	_T("SMB2j Pirate"),
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
