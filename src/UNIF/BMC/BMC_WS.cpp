/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
uint8_t Reg0, Reg1;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0x0, Reg1 & 0x7);
	if (Reg0 & 0x08)
	{
		EMU->SetPRG_ROM16(0x8, Reg0 & 0x7);
		EMU->SetPRG_ROM16(0xC, Reg0 & 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8, (Reg0 & 0x6) >> 1);
	if (Reg0 & 0x10)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Reg0);
	SAVELOAD_BYTE(mode, offset, data, Reg1);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Reg0 & 0x20)
		return;
	switch (Addr & 1)
	{
	case 0:	Reg0 = Val;	break;
	case 1:	Reg1 = Val;	break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6, Write);
	Reg0 = Reg1 = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_BMC_WS
(
	"BMC-WS",
	_T("Pirate multicart mapper"),
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
