/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Cmd, CHRH, CHRL0, CHRL1, CHRL2, CHRL3, PRG, CHRO, Mirror;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	if (Mirror & 1)
	{
		EMU->SetCHR_ROM2(0x0, (CHRH << 4) | (CHRL0 << 1) | 0);
		EMU->SetCHR_ROM2(0x2, (CHRH << 4) | (CHRL0 << 1) | 1);
		EMU->SetCHR_ROM2(0x4, (CHRH << 4) | (CHRL0 << 1) | 0);
		EMU->SetCHR_ROM2(0x6, (CHRH << 4) | (CHRL0 << 1) | 1);
	}
	else
	{
		EMU->SetCHR_ROM2(0x0, (CHRH << 4) | (CHRL0 << 1) | 0);
		EMU->SetCHR_ROM2(0x2, (CHRH << 4) | (CHRL1 << 1) | 1);
		EMU->SetCHR_ROM2(0x4, (CHRH << 4) | (CHRL2 << 1) | 0);
		EMU->SetCHR_ROM2(0x6, (CHRH << 4) | (CHRL3 << 1) | 1);
	}
	if (!ROM->INES_CHRSize)
		EMU->SetCHR_RAM8(0x0, 0);
	switch (Mirror >> 1)
	{
	case 0:	EMU->Mirror_Custom(0, 0, 0, 1);	break;
	case 1:	EMU->Mirror_A11();		break;
	case 2:	EMU->Mirror_A10();		break;
	case 3:	EMU->Mirror_S0();		break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Cmd);
	SAVELOAD_BYTE(mode, offset, data, CHRH);
	SAVELOAD_BYTE(mode, offset, data, CHRL0);
	SAVELOAD_BYTE(mode, offset, data, CHRL1);
	SAVELOAD_BYTE(mode, offset, data, CHRL2);
	SAVELOAD_BYTE(mode, offset, data, CHRL3);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHRO);
	SAVELOAD_BYTE(mode, offset, data, Mirror);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	Val &= 0x07;
	switch (Addr & 0x101)
	{
	case 0x100:
		Cmd = Val;	break;
	case 0x101:
		switch (Cmd)
		{
		case 0:	CHRL0 = Val;	break;
		case 1:	CHRL1 = Val;	break;
		case 2:	CHRL2 = Val;	break;
		case 3:	CHRL3 = Val;	break;
		case 4:	CHRH = Val;	break;
		case 5:	PRG = Val;	break;
		case 6:	CHRO = Val;	break;
		case 7:	Mirror = Val;	break;
		}			break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x6; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Cmd = CHRH = CHRL0 = CHRL1 = CHRL2 = CHRL3 = PRG = CHRO = Mirror = 0;

	Sync();
}
} // namespace

const MapperInfo MapperInfo_141
(
	141,
	_T("Sachen (SA8259A)"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
