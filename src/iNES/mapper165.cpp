/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper045.cpp $
 * $Id: mapper045.cpp 1077 2010-12-20 04:16:53Z quietust $
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
FPPURead _PPURead3, _PPURead7;
uint8 LatchState[2];

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncWRAM();
}

void	SyncCHR (void)
{
	int bank;
	if (LatchState[0])
		bank = MMC3::GetCHRBank(2);
	else	bank = MMC3::GetCHRBank(0);
	if (bank == 0)
		EMU->SetCHR_RAM4(0, 0);
	else	EMU->SetCHR_ROM4(0, bank);
	if (LatchState[1])
		bank = MMC3::GetCHRBank(6);
	else	bank = MMC3::GetCHRBank(4);
	if (bank == 0)
		EMU->SetCHR_RAM4(4, 0);
	else	EMU->SetCHR_ROM4(4, bank);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, LatchState[i]);
	if (mode == STATE_LOAD)
	{
		Sync();
		SyncCHR();
	}
	return offset;
}

int	MAPINT	PPURead3 (int Bank, int Addr)
{
	int result = _PPURead3(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		LatchState[0] = 0;
		SyncCHR();
	}
	else if (addy == 0x3E8)
	{
		LatchState[0] = 1;
		SyncCHR();
	}
	return result;
}

int	MAPINT	PPURead7 (int Bank, int Addr)
{
	int result = _PPURead7(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D8)
	{
		LatchState[1] = 0;
		SyncCHR();
	}
	else if (addy == 0x3E8)
	{
		LatchState[1] = 1;
		SyncCHR();
	}
	return result;
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);

	_PPURead3 = EMU->GetPPUReadHandler(0x3);
	_PPURead7 = EMU->GetPPUReadHandler(0x7);

	EMU->SetPPUReadHandler(0x3, PPURead3);
	EMU->SetPPUReadHandler(0x7, PPURead7);
	SyncCHR();
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 165;
} // namespace

const MapperInfo MapperInfo_165 =
{
	&MapperNum,
	_T("Fire Emblem"),
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