/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPURead _Read6;
FCPUWrite _Write6;
u8 PRG[4], CHR[4];

void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
	{
		EMU->SetPRG_ROM8(0x8 | (x << 1), PRG[x]);
		EMU->SetCHR_ROM2(x << 1, CHR[x]);
	}
	EMU->SetPRG_RAM4(0x6, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

int	MAPINT	Read6 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return _Read6(Bank, Addr & 0x7FF);
	else	return -1;
}

void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		_Write6(Bank, Addr & 0x7FF, Val);
		return;
	}
	switch (Addr & 0x007)
	{
	case 0:	PRG[0] = Val;	break;
	case 1:	PRG[1] = Val;	break;
	case 2:	PRG[2] = Val;	break;
	case 3:	PRG[3] = Val;	break;
	case 4:	CHR[0] = Val;	break;
	case 5:	CHR[1] = Val;	break;
	case 6:	CHR[2] = Val;	break;
	case 7:	CHR[3] = Val;	break;
	}
	Sync();
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	_Read6 = EMU->GetCPUReadHandler(0x6);
	_Write6 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUReadHandler(0x6, Read6);
	EMU->SetCPUWriteHandler(0x6, Write6);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;	PRG[1] = 1;
		PRG[2] = 0xFE;	PRG[3] = 0xFF;
		for (x = 0; x < 4; x++)
			CHR[x] = 0;
	}
	Sync();
}

u8 MapperNum = 246;
} // namespace

const MapperInfo MapperInfo_246 =
{
	&MapperNum,
	_T("Mapper 246"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};