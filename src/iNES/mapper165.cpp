/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace MMC3
{
extern uint8_t Mirror;
}
namespace
{
FPPURead _PPURead3, _PPURead7;
uint8_t LatchState[2];

void	SyncCHR (void)
{
	int bank;

	bank = MMC3::GetCHRBank(LatchState[0] ? 2 : 0) >> 2;
	if (bank == 0)
		EMU->SetCHR_RAM4(0x0, 0);
	else	EMU->SetCHR_ROM4(0x0, bank);

	bank = MMC3::GetCHRBank(LatchState[1] ? 6 : 4) >> 2;
	if (bank == 0)
		EMU->SetCHR_RAM4(0x4, 0);
	else	EMU->SetCHR_ROM4(0x4, bank);
}

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncWRAM();
	SyncCHR();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, LatchState[i]);

	if (IsLoad(mode))
		Sync();
	return offset;
}

int	MAPINT	PPURead3 (int Bank, int Addr)
{
	int result = _PPURead3(Bank, Addr);
	register int addy = Addr & 0x3F8;
	if (addy == 0x3D0)
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
	if (addy == 0x3D0)
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

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);

	_PPURead3 = EMU->GetPPUReadHandler(0x3);
	_PPURead7 = EMU->GetPPUReadHandler(0x7);

	EMU->SetPPUReadHandler(0x3, PPURead3);
	EMU->SetPPUReadHandler(0x7, PPURead7);
	EMU->SetPPUReadHandlerDebug(0x3, _PPURead3);
	EMU->SetPPUReadHandlerDebug(0x7, _PPURead7);
	
	// Fire Emblem seems to require mirroring to default to vertical
	if (ResetType == RESET_HARD)
		MMC3::Mirror = 0;
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint16_t MapperNum = 165;
} // namespace

const MapperInfo MapperInfo_165
(
	&MapperNum,
	_T("Fire Emblem"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
);
