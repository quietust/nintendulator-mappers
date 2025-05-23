/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8_t Reg0, Reg1, Reg2, Reg3;

void	Sync (void)
{
	int PRGmask, CHRmask;
	int PRGbank, CHRbank;

	if (Reg0 & 0x40)
	{
		PRGmask = 0x0F;
		PRGbank = (Reg0 & 0x7) << 4;
	}
	else
	{
		PRGmask = 0x1F;
		PRGbank = (Reg0 & 0x6) << 4;
	}
	PRGbank |= (Reg0 & 0x10) << 3;

	if (Reg0 & 0x80)
	{
		CHRmask = 0x7F;
		CHRbank = (Reg0 & 0x08) << 4;
	}
	else
	{
		CHRmask = 0xFF;
		CHRbank = 0;
	}
	CHRbank |= (Reg0 & 0x20) << 3;
	CHRbank |= (Reg0 & 0x10) << 5;

	if (Reg3 & 0x08)
		EMU->DbgOut(_T("Alternate PRG switching used in mapper 126!"));
	else
	{
		switch (Reg3 & 0x3)
		{
		case 0:	MMC3::SyncPRG(PRGmask, PRGbank);	break;
		case 1:
		case 2:	EMU->SetPRG_ROM16(0x8, ((MMC3::GetPRGBank(0) & PRGmask) | PRGbank) >> 1);
			EMU->SetPRG_ROM16(0xC, ((MMC3::GetPRGBank(0) & PRGmask) | PRGbank) >> 1);	break;
		case 3:	EMU->SetPRG_ROM32(0x8, ((MMC3::GetPRGBank(0) & PRGmask) | PRGbank) >> 2);	break;
		}
	}
	if (Reg3 & 0x10)
	{
		if (!(Reg0 & 0x80))
			CHRbank |= Reg2 & 0x80;
		EMU->SetCHR_ROM8(0x0, (Reg2 & 0xF) | (CHRbank >> 3));
	}
	else	MMC3::SyncCHR_ROM(CHRmask, CHRbank);
	MMC3::SyncMirror();
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	CheckSave(offset = MMC3::SaveLoad(mode, offset, data));
	SAVELOAD_BYTE(mode, offset, data, Reg0);
	SAVELOAD_BYTE(mode, offset, data, Reg1);
	SAVELOAD_BYTE(mode, offset, data, Reg2);
	SAVELOAD_BYTE(mode, offset, data, Reg3);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	if (Reg3 & 0x80)
			return;
		Reg0 = Val;	break;
	case 1:	Reg1 = Val;	break;
	case 2:	Reg2 = Val;	break;
	case 3:	if (Reg3 & 0x80)
			return;
		Reg3 = Val;	break;
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
	EMU->SetCPUWriteHandler(0x6, Write);
	EMU->SetCPUWriteHandler(0x7, Write);

	if (ResetType == RESET_HARD)
		Reg0 = Reg1 = Reg2 = Reg3 = 0;
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_126
(
	126,
	_T("Super Joy (MMC3)"),
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
