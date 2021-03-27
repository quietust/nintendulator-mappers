/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_N118.h"

namespace N118
{
extern uint8_t Cmd;
}
namespace
{
void	Sync (void)
{
	N118::SyncPRG();
	EMU->SetCHR_ROM2(0x0, N118::CHR[0] >> 1);
	EMU->SetCHR_ROM2(0x2, N118::CHR[1] >> 1);
	EMU->SetCHR_ROM1(0x4, N118::CHR[2] | 0x40);
	EMU->SetCHR_ROM1(0x5, N118::CHR[3] | 0x40);
	EMU->SetCHR_ROM1(0x6, N118::CHR[4] | 0x40);
	EMU->SetCHR_ROM1(0x7, N118::CHR[5] | 0x40);
	if (N118::Cmd & 0x40)
		EMU->Mirror_S1();
	else	EMU->Mirror_V();
}

BOOL	MAPINT	Load (void)
{
	N118::Load(Sync, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	N118::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N118::Unload();
}

uint16_t MapperNum = 154;
} // namespace

const MapperInfo MapperInfo_154
(
	&MapperNum,
	_T("Devil Man"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	N118::SaveLoad,
	NULL,
	NULL
);
