/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Mode;
uint8_t PRG;
uint8_t CHR[2];
FCPUWrite _Write7;

void	Sync_NINA (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_ROM4(0x0, CHR[0]);
	EMU->SetCHR_ROM4(0x4, CHR[1]);
}

void	Sync_BNROM (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_RAM8(0x0, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Mode);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	if (Mode == 1)
	{
		SAVELOAD_BYTE(mode, offset, data, CHR[0]);
		SAVELOAD_BYTE(mode, offset, data, CHR[1]);
	}

	if (IsLoad(mode))
	{
		if (Mode == 1)
			Sync_NINA();
		if (Mode == 2)
			Sync_BNROM();
	}
	return offset;
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

BOOL	MAPINT	Load (void)
{
	if (ROM->INES_CHRSize == 0)
		Mode = 2;
	else	Mode = 1;
	return TRUE;
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
		for (int i = 0x8; i < 0x10; i++)
			EMU->SetCPUWriteHandler(i, WriteBNROM);
		Sync_BNROM();
	}

}

uint16_t MapperNum = 34;
} // namespace

const MapperInfo MapperInfo_034
(
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
);
