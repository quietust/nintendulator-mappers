/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t Reg;

void	Sync (void)
{
	MMC3::SyncMirror();
	switch (Reg)
	{
	case 0:	case 1:	case 2:
		MMC3::SyncPRG(0x7, 0x00);
		break;
	case 3:
		MMC3::SyncPRG(0x7, 0x08);
		break;
	case 4:	case 5:	case 6:
		MMC3::SyncPRG(0xF, 0x10);
		break;
	case 7:
		MMC3::SyncPRG(0x7, 0x18);
		break;
	}
	MMC3::SyncCHR_ROM(0x7F, (Reg & 0x4) << 5);
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

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Reg = Val & 7;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);
	MMC3::Reset(ResetType);

	if (ResetType == RESET_HARD)
	{
		Reg = 0;
		Sync();
	}
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint16_t MapperNum = 37;
} // namespace

const MapperInfo MapperInfo_037
(
	&MapperNum,
	_T("Super Mario Bros & Tetris & Nintendo World Cup Soccer (MMC3)"),
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
