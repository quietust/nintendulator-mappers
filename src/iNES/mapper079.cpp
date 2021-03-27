/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPUWrite _Write4;
uint8_t Reg;

void	Sync (void)
{
	EMU->SetCHR_ROM8(0x0, Reg & 0x7);
	EMU->SetPRG_ROM32(0x8, (Reg & 0x8) >> 3);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Reg);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if (Addr & 0x100)
	{
		Reg = Val;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write);
	EMU->SetCPUWriteHandler(0x5, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;

	Sync();
}

uint16_t MapperNum = 79;
} // namespace

const MapperInfo MapperInfo_079
(
	&MapperNum,
	_T("NINA-03/NINA-06"),
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
