/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Cmd;
uint8_t PRG[3];
uint8_t CHR[4];
uint8_t Mirror;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, -1);
	for (int i = 0; i < 4; i++)
		EMU->SetCHR_ROM2(i << 1, CHR[i]);
	if (Mirror & 1)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Mirror);
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 0x1)
	{
	case 0:	Cmd = Val;	break;
	case 1:	switch (Cmd & 0x07)
		{
		case 2:	CHR[0] = Val;	break;
		case 3:	CHR[1] = Val;	break;
		case 4:	CHR[2] = Val;	break;
		case 5:	CHR[3] = Val;	break;
		case 6:	if (Cmd & 0x40)
				PRG[2] = Val;
			else	PRG[0] = Val;
					break;
		case 7:	PRG[1] = Val;	break;
		}			break;
	}
	Sync();
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 0x1)
	{
	case 0:	Mirror = Val;	break;
	case 1:			break;
	}
	Sync();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);

	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0;
		PRG[1] = 1;
		PRG[2] = 0xFE;
		for (int i = 0; i < 4; i++)
			CHR[i] = 0;
		Cmd = 0;
	}

	Sync();
}
} // namespace

const MapperInfo MapperInfo_076
(
	76,
	_T("Mapper 76"),
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
