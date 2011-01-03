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
uint8 PRG;
uint8 Valid;

void	Sync (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
	if (PRG & 0x80)
		EMU->SetPRG_ROM16(0x8, PRG & 0xF);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, Valid);
	offset = MMC3::SaveLoad(mode, offset, data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	PRG = Val;	break;
	}
	Sync();
}

void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3::CPUWriteAB(Bank, 0, Val);
}

void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3::CPUWrite89(Bank, 0, (Val & 0xC0) | LUT[Val & 0x7]);
	Valid = 1;
}

void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Valid)
		MMC3::CPUWrite89(Bank, 1, Val);
	Valid = 0;
}

void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Val)
	{
		MMC3::CPUWriteCD(Bank, 0, Val);
		MMC3::CPUWriteCD(Bank, 1, Val);
		MMC3::CPUWriteEF(Bank, 1, Val);
	}
	else	MMC3::CPUWriteEF(Bank, 0, Val);
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
	EMU->SetCPUWriteHandler(0x6, Write6);
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);
	if (ResetType == RESET_HARD)
	{
		PRG = 0x00;
		Valid = 1;
		Sync();
	}
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 114;
} // namespace

const MapperInfo MapperInfo_114 =
{
	&MapperNum,
	_T("Mapper 114"),
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