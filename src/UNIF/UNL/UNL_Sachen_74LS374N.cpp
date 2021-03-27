/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
uint8_t Cmd, PRG, CHR, Mirror;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, (PRG << 1) | 0);
	EMU->SetPRG_ROM16(0xC, (PRG << 1) | 1);
	EMU->SetCHR_ROM8(0x0, CHR);
	if (Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
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
	uint16_t Loc = (Bank << 12) | Addr;
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
				CHR = 3;			break;
			case 4:	CHR &= 0x6;
				CHR |= (Val & 0x1) << 0;	break;
			case 5:	PRG = Val & 0x7;		break;
			case 6:	CHR &= 0x1;
				CHR |= (Val & 0x3) << 1;	break;
			case 7:	Mirror = (Val & 1);		break;
			}		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);

	Cmd = 0;
	PRG = 0;
	CHR = 3;
	Mirror = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_UNL_Sachen_74LS374N
(
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
);
