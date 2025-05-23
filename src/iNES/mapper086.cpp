/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
uint8_t Reg;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, (Reg & 0x30) >> 4);
	EMU->SetCHR_ROM8(0x0, (Reg & 0x03) | ((Reg & 0x40) >> 4));
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

void	MAPINT	WriteBank (int Bank, int Addr, int Val)
{
	Reg = Val;
	Sync();
}
void	MAPINT	WriteSpeech (int Bank, int Addr, int Val)
{
	if ((Val & 0x30) != 0x20)
		return;
	switch (Val & 0xF)
	{
	case 0x0:EMU->StatusOut(_T("\"Strike!\""));	break;
	case 0x1:EMU->StatusOut(_T("\"Ball!\""));	break;
	case 0x2:EMU->StatusOut(_T("\"Time!\""));	break;
	case 0x3:EMU->StatusOut(_T("\"Out!\""));	break;
	case 0x4:EMU->StatusOut(_T("\"Safe!\""));	break;
	case 0x5:EMU->StatusOut(_T("\"Foul!\""));	break;
	case 0x6:EMU->StatusOut(_T("\"Fair!\""));	break;
	case 0x7:EMU->StatusOut(_T("\"You're out!\""));	break;
	case 0x8:EMU->StatusOut(_T("\"Play ball!\""));	break;
	case 0x9:EMU->StatusOut(_T("\"Ball 4!\""));	break;
	case 0xA:EMU->StatusOut(_T("\"Home Run!\""));	break;
	case 0xB:EMU->StatusOut(_T("\"New Pitcher\""));	break;
	case 0xC:EMU->StatusOut(_T("\"Ouch!\""));	break;
	case 0xD:EMU->StatusOut(_T("\"Dummy!\""));	break;
	case 0xE:EMU->StatusOut(_T("*crack*"));		break;
	case 0xF:EMU->StatusOut(_T("*cheer*"));		break;
	}
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6, WriteBank);
	EMU->SetCPUWriteHandler(0x7, WriteSpeech);

	if (ResetType == RESET_HARD)
		Reg = 0;

	Sync();
}
} // namespace

const MapperInfo MapperInfo_086
(
	86,
	_T("Mapper 86 (Jaleco)"),
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
