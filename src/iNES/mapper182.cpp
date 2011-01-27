/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
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
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncWRAM();
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3::CPUWriteAB(Bank, 0, Val);
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
	if (Addr & 1)
		;
	else	MMC3::CPUWrite89(Bank, 0, LUT[Val & 0x7] | (Val & 0xC0));
}
void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Addr & 1)
	{
		MMC3::CPUWriteCD(Bank, 0, Val);
		MMC3::CPUWriteCD(Bank, 1, Val);
	}
	else	MMC3::CPUWrite89(Bank, 1, Val);
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3::CPUWriteEF(Bank, 1, Val);
	else	MMC3::CPUWriteEF(Bank, 0, Val);
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	return TRUE;
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

uint8 MapperNum = 182;
} // namespace

const MapperInfo MapperInfo_182 =
{
	&MapperNum,
	_T("Super Donkey Kong"),
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