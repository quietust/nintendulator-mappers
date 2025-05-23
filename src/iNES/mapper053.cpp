/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

namespace
{
uint8_t Game;

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6, (((Game & 0xF) << 4) | 0xF) + 4);
	if (Game & 0x10)
	{
		EMU->SetPRG_ROM16(0x8, (((Game & 0xF) << 3) | (Latch::Data & 0x7)) + 2);
		EMU->SetPRG_ROM16(0xC, (((Game & 0xF) << 3) | 0x7) + 2);
	}
	else	EMU->SetPRG_ROM32(0x8, 0); // the EPROM at the end, now inconveniently at the BEGINNING
	EMU->SetCHR_RAM8(0x0, 0);
	if (Game & 0x20)
		EMU->Mirror_A11();
	else	EMU->Mirror_A10();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = Latch::SaveLoad_D(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Game);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Game & 0x10)
		return;
	Game = Val & 0x3F;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	Latch::Load(Sync, FALSE, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
		Game = 0;
	Latch::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	Latch::Unload();
}
} // namespace

const MapperInfo MapperInfo_053
(
	53,
	_T("Supervision 16-in-1"),
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
