/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

#define IRQ_CYCLES 341
namespace
{
uint8_t IRQenabled, IRQcounter;
uint8_n IRQlatch;
int16_t IRQcycles;
uint8_t PRGswap;
uint8_t PRG[2];
uint16_n CHR[8];
uint8_t Mirror;

enum VRC4MODE { VRC4MODE_NONE, VRC4MODE_A, VRC4MODE_B, VRC4MODE_C, VRC4MODE_D, VRC4MODE_E, VRC4MODE_F, VRC4MODE_21, VRC4MODE_23, VRC4MODE_25 };
VRC4MODE Mode = VRC4MODE_NONE;
bool forceVRC2 = false;

const int SwapAddr[4] = {0, 2, 1, 3};
int transAddr (int Addr)
{
	switch (Mode)
	{
	case VRC4MODE_A:	return (Addr & 0x6) >> 1;
	case VRC4MODE_B:	return SwapAddr[Addr & 0x3];
	case VRC4MODE_C:	return (Addr & 0xC0) >> 6;
	case VRC4MODE_D:	return SwapAddr[(Addr & 0xC) >> 2];
	case VRC4MODE_E:	return (Addr & 0xC) >> 2;
	case VRC4MODE_F:	return (Addr & 0x3);
	case VRC4MODE_21:	return ((Addr & 0x6) >> 1) | ((Addr & 0xC0) >> 6);
	case VRC4MODE_23:	return (Addr & 0x3) | ((Addr & 0xC) >> 2);
	case VRC4MODE_25:	return SwapAddr[(Addr & 0x3) | ((Addr & 0xC) >> 2)];
	}
	return Addr;
}

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
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch.b0);
	SAVELOAD_WORD(mode, offset, data, IRQcycles);
	SAVELOAD_BYTE(mode, offset, data, PRGswap);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
	{
		if (ver == 0)
		{
			// Load old data
			SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
			CHR[i].b1 = 0;
		}
		else	SAVELOAD_WORD(mode, offset, data, CHR[i].s0);
	}
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled & 2) && ((IRQenabled & 4) || ((IRQcycles -= 3) <= 0)))
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
	PRG[0] = Val & 0x1F;
	Sync();
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (forceVRC2)
		Mirror = Val & 0x1;
	else switch (transAddr(Addr) & 0x3)
	{
	case 0:	Mirror = Val & 0x3;	break;
	case 2:	PRGswap = Val & 0x2;	break;
	}
	Sync();
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0x1F;
	Sync();
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (transAddr(Addr) & 0x3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 1:	CHR[0].n1 = Val & 0xF;	break;
	case 2:	CHR[1].n0 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	if (!forceVRC2)
		switch (transAddr(Addr) & 0x3)
		{
		case 1:	CHR[0].n2 = (Val >> 4) & 0x1;	break;
		case 3:	CHR[1].n2 = (Val >> 4) & 0x1;	break;
		}
	Sync();
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (transAddr(Addr) & 0x3)
	{
	case 0:	CHR[2].n0 = Val & 0x0F;	break;
	case 1:	CHR[2].n1 = Val & 0x1F;	break;
	case 2:	CHR[3].n0 = Val & 0x0F;	break;
	case 3:	CHR[3].n1 = Val & 0x1F;	break;
	}
	if (!forceVRC2)
		switch (transAddr(Addr) & 0x3)
		{
		case 1:	CHR[2].n2 = (Val >> 4) & 0x1;	break;
		case 3:	CHR[3].n2 = (Val >> 4) & 0x1;	break;
		}
	Sync();
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (transAddr(Addr) & 0x3)
	{
	case 0:	CHR[4].n0 = Val & 0x0F;	break;
	case 1:	CHR[4].n1 = Val & 0x1F;	break;
	case 2:	CHR[5].n0 = Val & 0x0F;	break;
	case 3:	CHR[5].n1 = Val & 0x1F;	break;
	}
	if (!forceVRC2)
		switch (transAddr(Addr) & 0x3)
		{
		case 1:	CHR[4].n2 = (Val >> 4) & 0x1;	break;
		case 3:	CHR[5].n2 = (Val >> 4) & 0x1;	break;
		}
	Sync();
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	switch (transAddr(Addr) & 0x3)
	{
	case 0:	CHR[6].n0 = Val & 0x0F;	break;
	case 1:	CHR[6].n1 = Val & 0x1F;	break;
	case 2:	CHR[7].n0 = Val & 0x0F;	break;
	case 3:	CHR[7].n1 = Val & 0x1F;	break;
	}
	if (!forceVRC2)
		switch (transAddr(Addr) & 0x3)
		{
		case 1:	CHR[6].n2 = (Val >> 4) & 0x1;	break;
		case 3:	CHR[7].n2 = (Val >> 4) & 0x1;	break;
		}
	Sync();
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (forceVRC2)
		return;
	switch (transAddr(Addr) & 0x3)
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

BOOL	MAPINT	Load_21 (void)
{
	forceVRC2 = false;
	switch (ROM->INES2_SubMapper)
	{
	case 1:
		Mode = VRC4MODE_A;
		break;
	case 2:
		Mode = VRC4MODE_C;
		break;
	default:
		Mode = VRC4MODE_21;
		break;
	}
	iNES_SetSRAM();
	return TRUE;
}
BOOL	MAPINT	Load_23 (void)
{
	forceVRC2 = false;
	switch (ROM->INES2_SubMapper)
	{
	case 1:
		Mode = VRC4MODE_F;
		break;
	case 2:
		Mode = VRC4MODE_E;
		break;
	case 3:
		Mode = VRC4MODE_F;
		forceVRC2 = true;
		break;
	default:
		Mode = VRC4MODE_23;
		break;
	}
	iNES_SetSRAM();
	return TRUE;
}
BOOL	MAPINT	Load_25 (void)
{
	forceVRC2 = false;
	switch (ROM->INES2_SubMapper)
	{
	case 1:
		Mode = VRC4MODE_B;
		break;
	case 2:
		Mode = VRC4MODE_D;
		break;
	case 3:
		Mode = VRC4MODE_B;
		forceVRC2 = true;
		break;
	default:
		Mode = VRC4MODE_25;
		break;
	}
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
		IRQenabled = IRQcounter = IRQlatch.b0 = 0;
		IRQcycles = 0;
		PRGswap = 0;
		Mirror = 0;
		PRG[0] = 0;
		PRG[1] = 1;
		for (int i = 0; i < 8; i++)
			CHR[i].s0 = i;
	}

	Sync();
}

uint16_t MapperNum21 = 21;
uint16_t MapperNum23 = 23;
uint16_t MapperNum25 = 25;
} // namespace

const MapperInfo MapperInfo_021
(
	&MapperNum21,
	_T("Konami VRC4a / VRC4c"),
	COMPAT_NEARLY,
	Load_21,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_023
(
	&MapperNum23,
	_T("Konami VRC4e / VRC4f / VRC2b"),
	COMPAT_NEARLY,
	Load_23,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_025
(
	&MapperNum25,
	_T("Konami VRC4b / VRC4d / VRC2c"),
	COMPAT_NEARLY,
	Load_25,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
