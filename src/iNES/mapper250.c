/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	MMC3_SyncMirror();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0xFF,0);
}

static	void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3_CPUWrite89(Bank,Addr >> 10,Addr & 0xFF);
}
static	void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	MMC3_CPUWriteAB(Bank,Addr >> 10,Addr & 0xFF);
}
static	void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	MMC3_CPUWriteCD(Bank,Addr >> 10,Addr & 0xFF);
}
static	void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	MMC3_CPUWriteEF(Bank,Addr >> 10,Addr & 0xFF);
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 250;
CTMapperInfo	MapperInfo_250 =
{
	&MapperNum,
	_T("Time Diver Avenger (MMC3 Variant)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};