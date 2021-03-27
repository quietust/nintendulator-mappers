/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG[2];
uint8_n CHR[8];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, 0x7);
	for (int i = 0; i < 8; i++)
		EMU->SetCHR_ROM1(i, CHR[i].b0 >> 1);
	if (Mirror & 1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val & 0xF;
	Sync();
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mirror = Val & 0x3;
	Sync();
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0x1F;
	Sync();
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 2:	CHR[0].n1 = Val & 0xF;	break;
	case 1:	CHR[1].n0 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[2].n0 = Val & 0xF;	break;
	case 2:	CHR[2].n1 = Val & 0xF;	break;
	case 1:	CHR[3].n0 = Val & 0xF;	break;
	case 3:	CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[4].n0 = Val & 0xF;	break;
	case 2:	CHR[4].n1 = Val & 0xF;	break;
	case 1:	CHR[5].n0 = Val & 0xF;	break;
	case 3:	CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	CHR[6].n0 = Val & 0xF;	break;
	case 2:	CHR[6].n1 = Val & 0xF;	break;
	case 1:	CHR[7].n0 = Val & 0xF;	break;
	case 3:	CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
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

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;	PRG[1] = 1;
		for (int i = 0; i < 8; i++)
			CHR[i].b0 = i;
		Mirror = 0;
	}
	Sync();
}

uint16_t MapperNum = 22;
} // namespace

const MapperInfo MapperInfo_022
(
	&MapperNum,
	_T("Konami VRC2a"),
	COMPAT_NEARLY,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
