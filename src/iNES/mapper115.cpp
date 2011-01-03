/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
uint8 Reg1, Reg2;

void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncPRG(0x3F, 0);
	if (Reg1 & 0x80)
		EMU->SetPRG_ROM16(0x8, Reg1 & 0xF);
	MMC3::SyncCHR_ROM(0xFF, (Reg2 & 0x01) << 8);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, Reg1);
	SAVELOAD_BYTE(mode, offset, data, Reg2);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
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
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUWriteHandler(i, Write);
	if (ResetType == RESET_HARD)
		Reg1 = Reg2 = 0;
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 115;
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