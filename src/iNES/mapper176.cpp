/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_iNES.h"
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
	if ((BankSize & 0x7) == 5)
	{
		// UNROM mode
	}
	else	MMC3::SyncPRG(PRGmask[BankSize & 0x7], PRGbank << 1);

	int CHRmask = 0xFF;
	if (BankSize & 0x10)
		CHRmask &= ~0x80;
	if (BankSize & 0x40)
		CHRmask &= ~0x07;
	if (BankSize & 0x20)
		MMC3::SyncCHR_RAM(0x07, 0);
	else	MMC3::SyncCHR_ROM(CHRmask, CHRbank << 3);
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
	switch (Addr & 3)
	{
	case 0x0:	BankSize = Val;	break;
	case 0x1:	PRGbank = Val;	break;
	case 0x2:	CHRbank = Val;	break;
	}
	Sync();
}

BOOL	MAPINT	Load (void)
{
	// Nonzero submappers not supported yet
	if (ROM->INES2_SubMapper != 0)
	{
		EMU->DbgOut(_T("Only submapper 0 is currently supported"));
		return FALSE:
	}
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

const MapperInfo MapperInfo_176
(
	176,
	_T("Mapper 176"),
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
