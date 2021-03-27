/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

namespace
{
uint8_t BankSize;
uint8_t PRGbank, CHRbank;

void	Sync (void)
{
	int PRGmask[8] = {0x3F,0x1F,0x0F,0x01,0x03,0x00,0x00,0x00};
	MMC3::SyncMirror();
	MMC3::SyncWRAM();
	MMC3::SyncPRG(PRGmask[BankSize & 0x7], PRGbank << 1);
	if (BankSize & 0x20)
		MMC3::SyncCHR_RAM(0x07, 0);
	else	MMC3::SyncCHR_ROM(0xFF, CHRbank << 3);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, BankSize);
	SAVELOAD_BYTE(mode, offset, data, PRGbank);
	SAVELOAD_BYTE(mode, offset, data, CHRbank);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0xFF0:	BankSize = Val;	break;
	case 0xFF1:	PRGbank = Val;	break;
	case 0xFF2:	CHRbank = Val;	break;
	}
	Sync();
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x5, Write);

	if (ResetType == RESET_HARD)
	{
		BankSize = 0x24;
		PRGbank = 0x9F;
		CHRbank = 0;
	}
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_BMC_Super24in1SC03
(
	"BMC-Super24in1SC03",
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
);
