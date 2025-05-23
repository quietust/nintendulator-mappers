/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Reg;
FCPURead _Read4;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0x0, Reg & 0x3);
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

int	MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x100)
		return ((*EMU->OpenBus) & 0xF0) | Reg;
	else if (Bank == 4)
		return _Read4(Bank, Addr);
	else	return -1;
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

	_Read4 = EMU->GetCPUReadHandler(0x4);
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x6; i++)
	{
		EMU->SetCPUReadHandler(i, Read);
		EMU->SetCPUWriteHandler(i, Write);
	}

	if (ResetType == RESET_HARD)
		Reg = 0;
	Sync();
}
} // namespace

const MapperInfo MapperInfo_132
(
	132,
	_T("Creatom"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
