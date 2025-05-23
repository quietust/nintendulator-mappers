/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t IRQenabled, IRQcounter, IRQlatch, IRQreload;
uint8_t PRG[2];
uint8_t CHR[6];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM2(0x0, CHR[0]);
	EMU->SetCHR_ROM2(0x2, CHR[1]);
	EMU->SetCHR_ROM1(0x4, CHR[2]);
	EMU->SetCHR_ROM1(0x5, CHR[3]);
	EMU->SetCHR_ROM1(0x6, CHR[4]);
	EMU->SetCHR_ROM1(0x7, CHR[5]);
	if (Mirror)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, IRQcounter);
	SAVELOAD_BYTE(mode, offset, data, IRQlatch);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQreload);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((IsRendering) && (Cycle == 264))
	{
		unsigned char count = IRQcounter;
		if (!IRQcounter || IRQreload)
			IRQcounter = IRQlatch;
		else	IRQcounter--;
		if ((count || IRQreload) && !IRQcounter && IRQenabled) 
			EMU->SetIRQ(0);
		IRQreload = 0;
	}
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	PRG[0] = Val;	break;
	case 1:	PRG[1] = Val;	break;
	case 2:	CHR[0] = Val;	break;
	case 3:	CHR[1] = Val;	break;
	}
	Sync();
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[2] = Val;	break;
	case 1:	CHR[3] = Val;	break;
	case 2:	CHR[4] = Val;	break;
	case 3:	CHR[5] = Val;	break;
	}
	Sync();
}
void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	IRQlatch = Val ^ 0xFF;	break;
	case 1:	IRQcounter = 0;
		IRQreload = 1;		break;
	case 2:	IRQenabled = 1;		break;
	case 3:	IRQenabled = 0;
		EMU->SetIRQ(1);		break;
	}
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	Mirror = Val & 0x40;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);

	if (ResetType == RESET_HARD)
	{
		IRQenabled = IRQcounter = IRQlatch = IRQreload = 0;

		PRG[0] = 0;
		PRG[1] = 1;
		for (int i = 0; i < 6; i++)
			CHR[i] = i;
		Mirror = 0;
	}
	Sync();
}
} // namespace

const MapperInfo MapperInfo_048
(
	48,
	_T("Taito TC0190 with IRQs"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
