/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
uint8_t Mode;

void	Sync (void)
{
	EMU->SetCHR_RAM8(0x0, 0);
	if (Mode)
	{
		if (Latch::Data & 0x20)
		{
			EMU->SetPRG_ROM16(0x8, (Latch::Data & 0x1F) + 0x8);
			EMU->SetPRG_ROM16(0xC, (Latch::Data & 0x1F) + 0x8);
		}
		else	EMU->SetPRG_ROM32(0x8, ((Latch::Data & 0x1E) >> 1) + 0x4);
		if (Latch::Data & 0x40)
			EMU->Mirror_V();
		else	EMU->Mirror_H();
	}
	else
	{
		EMU->SetPRG_ROM16(0x8, Latch::Data & 0x7);
		EMU->SetPRG_ROM16(0xC, 0x7);
		EMU->Mirror_V();
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = Latch::SaveLoad_D(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Mode);

	if (IsLoad(mode))
		Sync();
	return offset;
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		Mode = 0;
	else	Mode ^= 1;

	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}

uint16_t MapperNum = 230;
} // namespace

const MapperInfo MapperInfo_230
(
	&MapperNum,
	_T("22-in-1"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
