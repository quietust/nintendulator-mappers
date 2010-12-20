/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
uint8 Cmd, PRG, CHR, Mirror;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, (PRG << 1) | 0);
	EMU->SetPRG_ROM16(0xC, (PRG << 1) | 1);
	EMU->SetCHR_ROM8(0, CHR);
	if (Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Cmd);
	SAVELOAD_BYTE(mode, x, data, PRG);
	SAVELOAD_BYTE(mode, x, data, CHR);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	uint16 Loc = (Bank << 12) | Addr;
	if (Loc < 0x4018)
	{
		_Write4(Bank, Addr, Val);
		return;
	}
	Val &= 0x07;
	switch (Loc & 0x4101)
	{
	case 0x4100:	Cmd = Val;	break;
	case 0x4101:	switch (Cmd)
			{
			case 0:	PRG = 0;
				CHR = 3;				break;
			case 4:	CHR &= 0x6;
				CHR |= (Val & 0x1) << 0;	break;
			case 5:	PRG = Val & 0x7;		break;
			case 6:	CHR &= 0x1;
				CHR |= (Val & 0x3) << 1;		break;
/*			case 4:	CHR &= 0x3;
				CHR |= Val << 2;	break;
			case 5:	PRG = Val;		break;
			case 6:	CHR &= 0x1C;
				CHR |= Val & 0x3;	break;*/
			case 7:	Mirror = (Val & 1);		break;
			}			break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);

	Cmd = 0;
	PRG = 0;
	CHR = 3;
	Mirror = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_UNL_Sachen_74LS374N =
{
	"UNL-Sachen-74LS374N",
	_T("Sachen mapper"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
