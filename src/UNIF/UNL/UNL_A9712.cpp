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
uint8 Reg0, Reg1;

void	Sync (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	if (Reg1 == 0xAB)
		EMU->SetPRG_ROM8(0xE, 0x07);
	if (Reg1 == 0xFF)
		EMU->SetPRG_ROM8(0xE, 0x09);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, Reg0);
	SAVELOAD_BYTE(mode, offset, data, Reg1);
	return offset;
}

int	MAPINT	Read5 (int Bank, int Addr)
{
	if (Addr & 0x80)
		return -1;
	else	return 0x9F;
}

void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr & 0x80)
		Reg0 = Val;
}
void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if ((Addr & 0x3) == 3)
	{
		Reg1 = Val;
		Sync();
	}
	else	MMC3::CPUWrite89(Bank, Addr, Val);
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
	EMU->SetCPUReadHandler(0x5, Read5);
	EMU->SetCPUWriteHandler(0x5, Write5);
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	if (ResetType == RESET_HARD)
	{
		Reg0 = 0;
		Reg1 = 0;
	}
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_UNL_A9712 =
{
	"UNL-A9712",
	_T("The Panda Prince pirate (with insane copy protection)"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
};