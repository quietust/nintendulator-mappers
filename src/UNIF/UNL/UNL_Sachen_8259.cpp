/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
u8 Cmd, CHR0l, CHR1l, CHR0h, CHR1h, CHRch, PRG, CHRmode, Mirror;
FCPUWrite _Write4;
FSync Sync;

void	Sync_8259A (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_RAM8(0, 0);	// in case there is no CHR ROM
	EMU->SetCHR_ROM2(0, 0 | (CHR0l << 1) | (CHRch << 4));
	EMU->SetCHR_ROM2(2, 1 | (CHR0h << 1) | (CHRch << 4));
	EMU->SetCHR_ROM2(4, 0 | (CHR1l << 1) | (CHRch << 4));
	EMU->SetCHR_ROM2(6, 1 | (CHR1h << 1) | (CHRch << 4));
	switch (Mirror >> 1)
	{
	case 0: EMU->Mirror_Custom(0, 0, 0, 1);	break;
	case 1: EMU->Mirror_H();		break;
	case 2: EMU->Mirror_V();		break;
	case 3: EMU->Mirror_S0();		break;
	}
}
void	Sync_8259B (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_RAM8(0, 0);	// in case there is no CHR ROM
	EMU->SetCHR_ROM2(0, CHR0l | (CHRch << 3));
	EMU->SetCHR_ROM2(2, CHR0h | (CHRch << 3));
	EMU->SetCHR_ROM2(4, CHR1l | (CHRch << 3));
	EMU->SetCHR_ROM2(6, CHR1h | (CHRch << 3));
	switch (Mirror >> 1)
	{
	case 0: EMU->Mirror_Custom(0, 0, 0, 1);	break;
	case 1: EMU->Mirror_H();		break;
	case 2: EMU->Mirror_V();		break;
	case 3: EMU->Mirror_S0();		break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Cmd);
	SAVELOAD_BYTE(mode, x, data, CHR0l);
	SAVELOAD_BYTE(mode, x, data, CHR0h);
	SAVELOAD_BYTE(mode, x, data, CHR1l);
	SAVELOAD_BYTE(mode, x, data, CHR1h);
	SAVELOAD_BYTE(mode, x, data, CHRch);
	SAVELOAD_BYTE(mode, x, data, PRG);
	SAVELOAD_BYTE(mode, x, data, CHRmode);
	SAVELOAD_BYTE(mode, x, data, Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	u16 Loc = (Bank << 12) | Addr;
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
			case 0: CHR0l = Val;	break;
			case 1: CHR0h = Val;	break;
			case 2: CHR1l = Val;	break;
			case 3: CHR1h = Val;	break;
			case 4: CHRch = Val;	break;
			case 5: PRG = Val;	break;
			case 6: CHRmode = Val;	break;
			case 7: Mirror = Val;	break;
			}			break;
	}
	Sync();
}

void	Reset (RESET_TYPE ResetType)
{
	u8 x;
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);
	Cmd = CHR0l = CHR1l = CHR0h = CHR1h = CHRch = PRG = Mirror = CHRmode = 0;
	Sync();
}
void	MAPINT	Reset_8259A (RESET_TYPE ResetType)
{
	Sync = Sync_8259A;
	Reset(ResetType);
}
void	MAPINT	Reset_8259B (RESET_TYPE ResetType)
{
	Sync = Sync_8259B;
	Reset(ResetType);
}
} // namespace

CTMapperInfo	MapperInfo_UNL_Sachen_8259A =
{
	"UNL-Sachen-8259A",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_8259A,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_Sachen_8259B =
{
	"UNL-Sachen-8259B",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_8259B,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};