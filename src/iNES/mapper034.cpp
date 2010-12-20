/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8 Mode;
uint8 PRG;
uint8 CHR[2];
FCPUWrite _Write7;

void	Sync_NINA (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_ROM4(0, CHR[0]);
	EMU->SetCHR_ROM4(4, CHR[1]);
}

void	Sync_BNROM (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_RAM8(0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Mode);
	SAVELOAD_BYTE(mode, x, data, PRG);
	if (Mode == 1)
	{
		SAVELOAD_BYTE(mode, x, data, CHR[0]);
		SAVELOAD_BYTE(mode, x, data, CHR[1]);
	}
	if (mode == STATE_LOAD)
	{
		if (Mode == 1)
			Sync_NINA();
		if (Mode == 2)
			Sync_BNROM();
	}
	return x;
}

void	MAPINT	WriteNINA (int Bank, int Addr, int Val)
{
	_Write7(Bank, Addr, Val);
	switch (Addr)
	{
	case 0xFFD:	PRG = Val;
			Sync_NINA();		break;
	case 0xFFE:	CHR[0] = Val;
			Sync_NINA();		break;
	case 0xFFF:	CHR[1] = Val;
			Sync_NINA();		break;
	}
}

void	MAPINT	WriteBNROM (int Bank, int Addr, int Val)
{
	PRG = Val;
	Sync_BNROM();
}

void	MAPINT	Load (void)
{
	if (ROM->INES_CHRSize == 0)
		Mode = 2;
	else	Mode = 1;
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		CHR[0] = 0;
		CHR[1] = 1;
	}

	if (Mode == 1)
	{
		_Write7 = EMU->GetCPUWriteHandler(0x7);
		EMU->SetCPUWriteHandler(0x7, WriteNINA);
		Sync_NINA();
	}
	if (Mode == 2)
	{
		for (int x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x, WriteBNROM);
		Sync_BNROM();
	}

}

uint8 MapperNum = 34;
} // namespace

const MapperInfo MapperInfo_034 =
{
	&MapperNum,
	_T("BNROM/Nina-01"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
