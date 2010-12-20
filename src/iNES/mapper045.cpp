/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8 Regs[4];
uint8 Pos;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(~Regs[3] & 0x3F, Regs[1]);
	MMC3::SyncCHR_ROM(0xFF >> ((~Regs[2]) & 0xF), (Regs[0]) | ((Regs[2] & 0xF0) << 4));
	MMC3::SyncWRAM();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, Regs[i]);
	SAVELOAD_BYTE(mode, offset, data, Pos);
	if (mode == STATE_LOAD)
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

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
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

uint8 MapperNum = 45;
} // namespace

const MapperInfo MapperInfo_045 =
{
	&MapperNum,
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
};