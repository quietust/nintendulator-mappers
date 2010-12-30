/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL: https://nintendulator.svn.sourceforge.net/svnroot/nintendulator/mappers/trunk/src/iNES/mapper004.cpp $
 * $Id: mapper004.cpp 1074 2010-12-20 03:30:32Z quietust $
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

// need access to these internals, since I don't feel like copy/pasting the entire MMC3 here
namespace MMC3
{
extern uint8 Cmd;
extern uint8 CHR[8];
}
namespace
{
void	Sync (void)
{
	MMC3::SyncMirror();
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x3F, (MMC3::CHR[0] & 0x2) << 5);
	if (ROM->INES_CHRSize)
	{
		EMU->SetCHR_RAM4(0, ((MMC3::Cmd & 0x80) >> 7) ^ 0);
		EMU->SetCHR_RAM4(4, ((MMC3::Cmd & 0x80) >> 7) ^ 1);
	}
	else	MMC3::SyncCHR_ROM(0xFF, 0);
}

void	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	iNES_SetSRAM();
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}

uint8 MapperNum = 245;
} // namespace

const MapperInfo MapperInfo_245 =
{
	&MapperNum,
	_T("Waixing MMC3 clone"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
};
