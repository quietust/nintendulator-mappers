/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_n PRG[3], CHR[8];
uint16_t IRQcounter;
uint16_n IRQlatch;
uint8_t IRQcontrol;
uint8_t Mirror;
uint8_t DisableSRAM;

uint16_t IRQmask;
void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	for (int i = 0; i < 3; i++)
		EMU->SetPRG_ROM8(0x8 | (i << 1), PRG[i].b0);

	EMU->SetPRG_ROM8(0xE, 0xFF);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i].b0);
	switch (Mirror)
	{
	case 0:	EMU->Mirror_A11();	break;
	case 1:	EMU->Mirror_A10();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (IRQcontrol & 0x8)
		IRQmask = 0xF;
	else if (IRQcontrol & 0x4)
		IRQmask = 0xFF;
	else if (IRQcontrol & 0x2)
		IRQmask = 0xFFF;
	else	IRQmask = 0xFFFF;
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i].b0);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
	SAVELOAD_WORD(mode, offset, data, IRQcounter);
	SAVELOAD_WORD(mode, offset, data, IRQlatch.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQcontrol);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	SAVELOAD_BYTE(mode, offset, data, DisableSRAM);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQcontrol & 1)
	{
		if (!(IRQcounter & IRQmask))
			EMU->SetIRQ(0);
		IRQcounter--;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	PRG[0].n0 = Val & 0xF;	break;
	case 1:	PRG[0].n1 = Val & 0xF;	break;
	case 2:	PRG[1].n0 = Val & 0xF;	break;
	case 3:	PRG[1].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	PRG[2].n0 = Val & 0xF;	break;
	case 1:	PRG[2].n1 = Val & 0xF;	break;
	case 2:	DisableSRAM = Val & 1;	break;
	case 3:					break;
	}
	Sync();
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 1:	CHR[0].n1 = Val & 0xF;	break;
	case 2:	CHR[1].n0 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[2].n0 = Val & 0xF;	break;
	case 1:	CHR[2].n1 = Val & 0xF;	break;
	case 2:	CHR[3].n0 = Val & 0xF;	break;
	case 3:	CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[4].n0 = Val & 0xF;	break;
	case 1:	CHR[4].n1 = Val & 0xF;	break;
	case 2:	CHR[5].n0 = Val & 0xF;	break;
	case 3:	CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[6].n0 = Val & 0xF;	break;
	case 1:	CHR[6].n1 = Val & 0xF;	break;
	case 2:	CHR[7].n0 = Val & 0xF;	break;
	case 3:	CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	IRQlatch.n0 = Val & 0xF;	break;
	case 1:	IRQlatch.n1 = Val & 0xF;	break;
	case 2:	IRQlatch.n2 = Val & 0xF;	break;
	case 3:	IRQlatch.n3 = Val & 0xF;	break;
	}
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	IRQcounter = IRQlatch.s0;
		EMU->SetIRQ(1);			break;
	case 1:	IRQcontrol = Val & 0xF;
		Sync();
		EMU->SetIRQ(1);			break;
	case 2:	Mirror = Val & 0x03;
		Sync();				break;
	case 3:					break;
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
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		PRG[0].b0 = 0x00;
		PRG[1].b0 = 0x01;
		PRG[2].b0 = 0xFE;
		for (int i = 0; i < 8; i++)
			CHR[i].b0 = i;
		IRQcontrol = 0;
		IRQcounter = IRQlatch.s0 = 0;
		Mirror = 0;
		DisableSRAM = 1;
	}
	Sync();
}
} // namespace

const MapperInfo MapperInfo_018
(
	18,
	_T("Jaleco SS8806"),
	COMPAT_NEARLY,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
