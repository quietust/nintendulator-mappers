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
u8 Reg1, Reg2;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	if (Reg1 & 0x80)
		EMU->SetPRG_ROM16(0x8, Reg1 & 0xF);
	MMC3::SyncCHR_ROM(0xFF, (Reg2 & 0x01) << 8);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3::SaveLoad(mode, x, data);
	SAVELOAD_BYTE(mode, x, data, Reg1);
	SAVELOAD_BYTE(mode, x, data, Reg2);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		Reg2 = Val;
	else	Reg1 = Val;
	Sync();
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);
	if (ResetType == RESET_HARD)
		Reg1 = Reg2 = 0;
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

u8 MapperNum = 115;
} // namespace

const MapperInfo MapperInfo_115 =
{
	&MapperNum,
	_T("Mapper 115 (Expanded Pirate MMC3)"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
};