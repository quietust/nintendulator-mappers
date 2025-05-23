/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Reg;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0x0, (Reg & 0x78) >> 3);
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
	if ((Addr & 0x103) == 0x102)
		Reg = Val & 0xF;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);
	for (int i = 0xC; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_147
(
	147,
	_T("Sachen (TC-U01-1.5M)"),
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
