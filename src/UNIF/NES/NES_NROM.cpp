/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace
{
void	MAPINT	Reset_NROM_128 (RESET_TYPE ResetType)
{
	EMU->SetPRG_ROM16(0x8, 0);
	EMU->SetPRG_ROM16(0xC, 0);

	if (ROM->UNIF_CHRSize[0])
		EMU->SetCHR_ROM8(0, 0);
	else	EMU->SetCHR_RAM8(0, 0);

	UNIF_SetMirroring(NULL);
}

void	MAPINT	Reset_RROM_128 (RESET_TYPE ResetType)
{
	EMU->SetPRG_ROM16(0x8, 0);
	EMU->SetPRG_ROM16(0xC, 0);

	if (ROM->UNIF_CHRSize[0])
		EMU->SetCHR_ROM8(0, 0);
	else	EMU->SetCHR_RAM8(0, 0);

	UNIF_SetMirroring(NULL);
}

void	MAPINT	Reset_NROM_256 (RESET_TYPE ResetType)
{
	EMU->SetPRG_ROM32(0x8, 0);

	if (ROM->UNIF_CHRSize[0])
		EMU->SetCHR_ROM8(0, 0);
	else	EMU->SetCHR_RAM8(0, 0);

	UNIF_SetMirroring(NULL);
}
} // namespace

const MapperInfo MapperInfo_NES_NROM_128 =
{
	"NES-NROM-128",
	_T("No mapper"),
	COMPAT_FULL,
	NULL,
	Reset_NROM_128,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_RROM_128 =
{
	"NES-RROM-128",
	_T("No mapper"),
	COMPAT_FULL,
	NULL,
	Reset_RROM_128,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_NROM_256 =
{
	"NES-NROM-256",
	_T("No mapper"),
	COMPAT_FULL,
	NULL,
	Reset_NROM_256,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};