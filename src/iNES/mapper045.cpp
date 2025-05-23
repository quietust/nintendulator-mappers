/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t Regs[4];
uint8_t Pos;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(~Regs[3] & 0x3F, Regs[1]);
	MMC3::SyncCHR_ROM(0xFF >> ((~Regs[2]) & 0xF), (Regs[0]) | ((Regs[2] & 0xF0) << 4));
	MMC3::SyncWRAM();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, Regs[i]);
	SAVELOAD_BYTE(mode, offset, data, Pos);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	MMC3::CPUWrite67(Bank, Addr, Val);
	if (Regs[3] & 0x40)
		return;
	Regs[Pos++] = Val;
	Pos &= 0x03;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		for (int i = 0; i < 4; i++)
			Regs[i] = 0;
		Pos = 0;
	}
	MMC3::Reset(ResetType);

	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_045
(
	45,
	_T("Super 1,000,000 in 1 (MMC3)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
