/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t WhichGame;
uint8_t DidWrite;
FCPUWrite _Write67;

void	Sync (void)
{
	int PRGmask, CHRmask;
	int PRGbank, CHRbank;

	if (WhichGame & 0x8)
	{
		PRGmask = 0x0F;
		PRGbank = WhichGame & 0x07;
	}
	else
	{
		PRGmask = 0x1F;
		PRGbank = WhichGame & 0x06;
	}
	if (WhichGame & 0x40)
	{
		CHRmask = 0x7F;
		CHRbank = ((WhichGame & 0x20) >> 3) | ((WhichGame & 0x04) >> 1) | ((WhichGame & 0x10) >> 4);
	}
	else
	{
		CHRmask = 0xFF;
		CHRbank = ((WhichGame & 0x20) >> 3) | ((WhichGame & 0x04) >> 1);
	}
	MMC3::SyncMirror();
	MMC3::SyncPRG(PRGmask, PRGbank << 4);
	MMC3::SyncWRAM();
	MMC3::SyncCHR_ROM(CHRmask, CHRbank << 7);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, WhichGame);
	SAVELOAD_BYTE(mode, offset, data, DidWrite);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	_Write67(Bank, Addr, Val);
	if (DidWrite)
		return;
	WhichGame = Val;
	DidWrite = 1;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
	_Write67 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
	{
		WhichGame = DidWrite = 0;
		Sync();
	}
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint16_t MapperNum = 52;
} // namespace

const MapperInfo MapperInfo_052
(
	&MapperNum,
	_T("Mario 7 in 1 (MMC3)"),
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
