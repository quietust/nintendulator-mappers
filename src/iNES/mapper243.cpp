/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Cmd, PRG, CHR, Mirror;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_ROM8(0x0, CHR);
	switch (Mirror)
	{
	case 0:	EMU->Mirror_A11();		break;
	case 1:	EMU->Mirror_A10();		break;
	case 2:	EMU->Mirror_Custom(0, 1, 1, 1);	break;
	case 3:	EMU->Mirror_S1();		break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Cmd);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHR);
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
		case 2:	CHR &= 0x7;
			CHR |= (Val & 0x1) << 3;	break;
		case 4:	CHR &= 0xB;
			CHR |= (Val & 0x1) << 2;	break;
		case 5:	PRG = Val & 0x7;		break;
		case 6:	CHR &= 0xC;
			CHR |= Val & 0x3;		break;
		case 7:	Mirror = (Val & 0x6) >> 1;	break;
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
	{
		Cmd = 0;
		PRG = 0;
		CHR = 3;
		Mirror = 0;
	}

	Sync();
}
} // namespace

const MapperInfo MapperInfo_243
(
	243,
	_T("Sachen (74LS374N)"),
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
