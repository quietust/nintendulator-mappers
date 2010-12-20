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
void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncWRAM();
	MMC3::SyncCHR_ROM(0xFF, 0);
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3::CPUWrite89(Bank, Addr >> 10, Addr & 0xFF);
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	MMC3::CPUWriteAB(Bank, Addr >> 10, Addr & 0xFF);
}
void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	MMC3::CPUWriteCD(Bank, Addr >> 10, Addr & 0xFF);
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	MMC3::CPUWriteEF(Bank, Addr >> 10, Addr & 0xFF);
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);

	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

u8 MapperNum = 250;
} // namespace

const MapperInfo MapperInfo_250 =
{
	&MapperNum,
	_T("Time Diver Avenger (MMC3 Variant)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
};