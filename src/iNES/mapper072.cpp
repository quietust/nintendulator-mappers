/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t PRG, CHR, Cmd;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	EMU->SetCHR_ROM8(0x0, CHR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHR);
	SAVELOAD_BYTE(mode, offset, data, Cmd);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Val & 0xC0)
	{
	case 0x00:
		switch (Cmd & 0xC0)
		{
		case 0x40:	CHR = Cmd & 0xF;	break;
		case 0x80:	PRG = Cmd & 0xF;	break;
		}		break;
	case 0x40:	case 0x80:
		Cmd = Val;	break;
	case 0xC0:		break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		PRG = CHR = Cmd = 0;

	Sync();
}

uint16_t MapperNum = 72;
} // namespace

const MapperInfo MapperInfo_072
(
	&MapperNum,
	_T("Pinball Quest"),
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
