/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
u8 Regs[4];
u8 Pos;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(~Regs[3] & 0x3F, Regs[1]);
	MMC3::SyncCHR_ROM(0xFF >> ((~Regs[2]) & 0xF), (Regs[0]) | ((Regs[2] & 0xF0) << 4));
	MMC3::SyncWRAM();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	x = MMC3::SaveLoad(mode, x, data);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, x, data, Regs[i]);
	SAVELOAD_BYTE(mode, x, data, Pos);
	if (mode == STATE_LOAD)
		Sync();
	return x;
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
	u8 x;
	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			Regs[x] = 0;
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

u8 MapperNum = 45;
} // namespace

CTMapperInfo	MapperInfo_045 =
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