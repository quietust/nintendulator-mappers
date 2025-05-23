/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Reg0, Reg1;

void	Sync (void)
{
	if (Reg0 & 0x20)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
	EMU->SetPRG_ROM32(0x8, Reg0 & 0x7);
	EMU->SetCHR_ROM8(0x0, Reg1 | ((Reg0 >> 1) & 0xC));
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

void	MAPINT	WriteLo (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		return;
	Reg0 = Addr & 0x3F;
	Sync();
}
void	MAPINT	WriteHi (int Bank, int Addr, int Val)
{
	if (Reg0 & 0x4)
	{
		Reg1 = Val & 0x03;
		Sync();
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6, WriteLo);
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, WriteHi);

	if (ResetType == RESET_HARD)
		Reg0 = Reg1 = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_041
(
	41,
	_T("Caltron 6-in-1"),
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
