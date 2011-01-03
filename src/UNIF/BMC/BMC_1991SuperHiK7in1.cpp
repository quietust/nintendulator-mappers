/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

namespace
{
uint8 WhichGame;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG((WhichGame == 6) ? 0x1F : 0x0F, WhichGame << 4);
	MMC3::SyncCHR_ROM((WhichGame == 6) ? 0xFF : 0x7F, WhichGame << 7);
	MMC3::SyncWRAM();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, WhichGame);
	return offset;
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	MMC3::CPUWriteAB(Bank, Addr, Val);	break;
	case 1:	WhichGame = Val & 0x07;
		if (WhichGame == 7)
			WhichGame = 6;
		Sync();				break;
	}
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
		WhichGame = 0;
	MMC3::Reset(ResetType);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_BMC_1991SuperHiK7in1 =
{
	"BMC-1991SuperHiK7in1",
	_T("Pirate multicart mapper"),
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