/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t Reg;
FCPUWrite _Write4;

void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8, Reg);
	MMC3::SyncWRAM();
	MMC3::SyncCHR_ROM(0x3F, 0);
	MMC3::SyncMirror();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Reg);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	_Write4(Bank, Addr, Val);
	if (Addr == 0x120)
	{
		Reg = Val >> 4;
		Sync();
	}
}

BOOL	MAPINT	Load (void)
{
	iNES_SetSRAM();
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write4);

	if (ResetType == RESET_HARD)
		Reg = 0;

	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint16_t MapperNum = 189;
} // namespace

const MapperInfo MapperInfo_189
(
	&MapperNum,
	_T("Mapper 189 (Thunder Warrior)"),
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
