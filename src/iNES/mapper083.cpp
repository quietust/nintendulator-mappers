/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Mode;
uint8_t PRG[5];
uint8_t CHR[8];
uint16_n IRQcounter;

void	Sync (void)
{
	if (Mode & 0x10)
	{
		if (Mode & 0x20)
			EMU->SetPRG_ROM8(0x6, PRG[3]);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
		EMU->SetPRG_ROM8(0x8, PRG[0]);
		EMU->SetPRG_ROM8(0xA, PRG[1]);
		EMU->SetPRG_ROM8(0xC, PRG[2]);
		EMU->SetPRG_ROM8(0xE, 0x1F);
	}
	else
	{
		if (Mode & 0x20)
			EMU->SetPRG_ROM8(0x6, 0x1F);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
		if (Mode & 0x08)
			EMU->SetPRG_ROM16(0x8, PRG[4]);
		else
		{
			EMU->SetPRG_ROM16(0x8, PRG[4]);
			EMU->SetPRG_ROM16(0xC, 0xF);
		}

	}
	if (ROM->INES_CHRSize > 32)
	{
		EMU->SetCHR_ROM2(0x0, CHR[0]);
		EMU->SetCHR_ROM2(0x2, CHR[1]);
		EMU->SetCHR_ROM2(0x4, CHR[6]);
		EMU->SetCHR_ROM2(0x6, CHR[7]);
	}
	else
	{
		for (int i = 0; i < 8; i++)
			EMU->SetCHR_ROM1(i, CHR[i]);
	}
	switch (Mode & 0x3)
	{
	case 0:	EMU->Mirror_A10();	break;
	case 1:	EMU->Mirror_A11();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Mode);
	for (int i = 0; i < 5; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQcounter.s0) && (Mode & 0x80))
	{
		if (Mode & 0x40)
			IRQcounter.s0--;
		else	IRQcounter.s0++;
		if (!IRQcounter.s0)
			EMU->SetIRQ(0);
	}
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch ((Addr >> 8) & 0x3)
	{
	case 0:	PRG[4] = Val;
		break;
	case 1:	Mode = Val;
		if ((Mode & 0x80) && (IRQcounter.s0 == 0))
			EMU->SetIRQ(0);
		else	EMU->SetIRQ(1);
		break;
	case 2:	if (Addr & 1)
			IRQcounter.b1 = Val;
		else	IRQcounter.b0 = Val;
		EMU->SetIRQ(1);
		break;
	case 3:	if (Addr & 0x10)
		{
			if (!(Addr & 0x8))
				CHR[Addr & 0x7] = Val;
		}
		else	PRG[Addr & 0x3] = Val & 0x1F;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write);

	if (ResetType == RESET_HARD)
	{
		Mode = 0;
		for (int i = 0; i < 8; i++)
			CHR[i] = 0;
		for (int i = 0; i < 5; i++)
			PRG[i] = 0;
		IRQcounter.s0 = 0;
	}

	Sync();
}
} // namespace

const MapperInfo MapperInfo_083
(
	83,
	_T("Cony"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
