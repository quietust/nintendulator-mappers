/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
FCPURead _Read4;
FCPUWrite _Write4;
uint8_t RegP, RegM, RegR;
uint8_t PRG, CHR;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, PRG);
	EMU->SetCHR_ROM8(0x0, CHR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHR);
	SAVELOAD_BYTE(mode, offset, data, RegP);
	SAVELOAD_BYTE(mode, offset, data, RegM);
	SAVELOAD_BYTE(mode, offset, data, RegR);

	if (IsLoad(mode))
		Sync();
	return offset;
}

int	MAPINT	Read4 (int Bank, int Addr)
{
	if (Addr & 0x100)
		return (*EMU->OpenBus & 0xCF) | ((RegR & 3) << 4);
	else if (Bank == 4)
		return _Read4(Bank, Addr);
	else	return -1;
}

void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if (Addr & 0x200)
		CHR = Val & 0xF;
	if (Addr & 0x100)
	{
		switch (Addr & 3)
		{
		case 0:	if (RegM)
				RegR = (RegR + 1) & 3;
			else	RegR = RegP;
			break;
		case 2:
			RegP = (Val >> 4) & 0x3;
			PRG = RegP;
			break;
		case 3:
			RegM = (Val >> 4) & 0x1;
			break;
		}
	}
	Sync();
}

void	MAPINT	Write8F (int Bank, int Addr, int Val)
{
	PRG = RegR;
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	_Read4 = EMU->GetCPUReadHandler(0x4);
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	for (int i = 0x4; i < 0x6; i++)
	{
		EMU->SetCPUReadHandler(i, Read4);
		EMU->SetCPUWriteHandler(i, Write4);
	}
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write8F);

	if (ResetType == RESET_HARD)
	{
		PRG = CHR = 0;
		RegP = RegR = RegM = 0;
	}
	Sync();
}

uint16_t MapperNum = 36;
} // namespace

const MapperInfo MapperInfo_036
(
	&MapperNum,
	_T("TXC 01-22000-400"),
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
