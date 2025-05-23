/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t IRQenabled;
uint16_t IRQcounter;
uint16_n IRQlatch;
uint8_t PRG[3];
uint8_t CHR[8];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	for (int i = 0; i < 3; i++)
		EMU->SetPRG_ROM8(0x8 | (i << 1), PRG[i]);
	EMU->SetPRG_ROM8(0xE, -1);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i]);
	if (Mirror & 0x80)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_WORD(mode, offset, data, IRQcounter);
	SAVELOAD_WORD(mode, offset, data, IRQlatch.s0);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled) && (!--IRQcounter))
	{
		EMU->SetIRQ(0);
		IRQenabled = 0;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	PRG[0] = Val;
		Sync();			break;
	}
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 1:	Mirror = Val;
		Sync();			break;
	case 3:	IRQenabled = Val & 0x80;
		EMU->SetIRQ(1);		break;
	case 4:	IRQcounter = IRQlatch.s0;
		EMU->SetIRQ(1);		break;
	case 5:	IRQlatch.b1 = Val;	break;
	case 6:	IRQlatch.b0 = Val;	break;
	}
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	PRG[1] = Val;
		Sync();			break;
	}
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	CHR[0] = Val;	break;
	case 1:	CHR[1] = Val;	break;
	case 2:	CHR[2] = Val;	break;
	case 3:	CHR[3] = Val;	break;
	case 4:	CHR[4] = Val;	break;
	case 5:	CHR[5] = Val;	break;
	case 6:	CHR[6] = Val;	break;
	case 7:	CHR[7] = Val;	break;
	}
	Sync();
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	PRG[2] = Val;
		Sync();			break;
	}
}

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;	PRG[1] = 1;	PRG[2] = 0xFE;
		for (int i = 0; i < 8; i++)
			CHR[i] = i;
		IRQenabled = 0;
		IRQcounter = IRQlatch.s0 = 0;
	}

	Sync();
}
} // namespace

const MapperInfo MapperInfo_065
(
	65,
	_T("Irem H-3001"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
