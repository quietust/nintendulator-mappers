/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPURead _Read;
uint8_t Reg1, Reg2, Reg3;

void	Sync (void)
{
	if (Reg1 & 0x80)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (Reg1 & 0x40)
	{
		EMU->SetPRG_ROM32(0x8, (Reg1 & 0x0E) | (Reg2 & 0x01));
		EMU->SetCHR_ROM8(0x0, ((Reg1 & 0x0E) << 2) | ((Reg2 & 0x70) >> 4));
	}
	else
	{
		EMU->SetPRG_ROM32(0x8, Reg1 & 0x0F);
		EMU->SetCHR_ROM8(0x0, ((Reg1 & 0x0F) << 2) | ((Reg2 & 0x30) >> 4));
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Reg1);
	SAVELOAD_BYTE(mode, offset, data, Reg2);
	SAVELOAD_BYTE(mode, offset, data, Reg3);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xFF8)
	{
	case 0xF80:	case 0xF88:	case 0xF90:	case 0xF98:
		if (Reg1 & 0x3F)
			break;
		Reg1 = Val;
		Sync();
		break;

	case 0xFA0:	case 0xFA8:	case 0xFB0:	case 0xFB8:
		// nothing here
		break;

	case 0xFC0:	case 0xFC8:	case 0xFD0:	case 0xFD8:
		if (Reg1 & 0x3F)
			break;
		// just lockout defeat, but we'll store it anyways
		Reg3 = Val & 0x3;
		Sync();
		break;

	case 0xFE0:
		break;

	case 0xFE8:	case 0xFF0:
		Reg2 = Val;
		Sync();
		break;

	case 0xFF8:
		break;
	}
}

int	MAPINT	Read (int Bank, int Addr)
{
	int Val = _Read(Bank, Addr);
	if ((Addr & 0xF80) == 0xF80)
		Write(Bank, Addr, Val);
	return Val;
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Read = EMU->GetCPUReadHandler(0xF);
	EMU->SetCPUReadHandler(0xF, Read);
	EMU->SetCPUReadHandlerDebug(0xF, _Read);
	EMU->SetCPUWriteHandler(0xF, Write);

	if (ResetType == RESET_HARD)
		Reg1 = Reg2 = Reg3 = 0;
	Sync();
}

uint16_t MapperNum = 234;
} // namespace

const MapperInfo MapperInfo_234
(
	&MapperNum,
	_T("Maxi 15"),
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
