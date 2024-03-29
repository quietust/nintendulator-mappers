/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC1.h"

namespace
{
void	Sync_SAROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x7, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
	MMC1::SyncWRAM();
}
void	Sync_SBROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x7, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
}
void	Sync_SCROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x7, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
}
void	Sync_SEROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x1, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
}
void	Sync_SFROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
}
void	Sync_SGROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_RAM(0x1, 0);
}
void	Sync_SHROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x1, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
}
void	Sync_SIROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x1, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
	MMC1::SyncWRAM();
}
void	Sync_SJROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_ROM(0x0F, 0);
	MMC1::SyncWRAM();
}
void	Sync_SKROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
	MMC1::SyncWRAM();
}
void	Sync_SLROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
}
void	Sync_SMROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_RAM(0x1, 0);
}
void	Sync_SNROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_RAM(0x01, 0);
	MMC1::SyncWRAM();
}
void	Sync_SOROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, 0);
	MMC1::SyncCHR_RAM(0x01, 0);
	if (MMC1::GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6, ((MMC1::GetCHRBankLo() & 0x08) >> 3) ^ 1);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}
void	Sync_SUROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, MMC1::GetCHRBankLo() & 0x10);
	MMC1::SyncCHR_RAM(0x01, 0);
	MMC1::SyncWRAM();
}
void	Sync_SXROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0xF, MMC1::GetCHRBankLo() & 0x10);
	MMC1::SyncCHR_RAM(0x01, 0);
	if (MMC1::GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6, (MMC1::GetCHRBankLo() & 0x0C) >> 2);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}

BOOL	MAPINT	Load_SAROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SAROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SBROM (void)
{
	MMC1::Load(Sync_SBROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SCROM (void)
{
	MMC1::Load(Sync_SCROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SEROM (void)
{
	MMC1::Load(Sync_SEROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SFROM (void)
{
	MMC1::Load(Sync_SFROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SGROM (void)
{
	MMC1::Load(Sync_SGROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SHROM (void)
{
	MMC1::Load(Sync_SHROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SIROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SIROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SJROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SJROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SKROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SLROM (void)
{
	MMC1::Load(Sync_SLROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SMROM (void)
{
	MMC1::Load(Sync_SGROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SNROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SNROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SOROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SOROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SUROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SUROM, TRUE, FALSE);
	return TRUE;
}
BOOL	MAPINT	Load_SXROM (void)
{
	UNIF_SetSRAM(32768);
	MMC1::Load(Sync_SXROM, TRUE, FALSE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC1::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC1::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_SAROM
(
	"NES-SAROM",
	_T("MMC1 with SRAM"),
	COMPAT_FULL,
	Load_SAROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SBROM
(
	"NES-SBROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SBROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SCROM
(
	"NES-SCROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SCROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SC1ROM
(
	"NES-SC1ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SCROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SEROM
(
	"NES-SEROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SEROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SFROM
(
	"NES-SFROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SFROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SGROM
(
	"NES-SGROM",
	_T("MMC1 with CHR-RAM"),
	COMPAT_FULL,
	Load_SGROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SHROM
(
	"NES-SHROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SHROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SH1ROM
(
	"NES-SH1ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SHROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SIROM
(
	"NES-SIROM",
	_T("MMC1 with CHR-ROM and SRAM"),
	COMPAT_FULL,
	Load_SKROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SJROM
(
	"NES-SJROM",
	_T("MMC1 with CHR-ROM and SRAM"),
	COMPAT_FULL,
	Load_SKROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SKROM
(
	"NES-SKROM",
	_T("MMC1 with CHR-ROM and SRAM"),
	COMPAT_FULL,
	Load_SKROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SLROM
(
	"NES-SLROM",
	_T("MMC1 with CHR-ROM"),
	COMPAT_FULL,
	Load_SLROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SL1ROM
(
	"NES-SL1ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SLROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SL2ROM
(
	"NES-SL2ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SLROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SL3ROM
(
	"NES-SL3ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SLROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SLRROM
(
	"NES-SLRROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SLROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SMROM
(
	"NES-SMROM",
	_T("MMC1 with CHR-RAM"),
	COMPAT_FULL,
	Load_SMROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SNROM
(
	"NES-SNROM",
	_T("MMC1 with CHR-RAM and SRAM"),
	COMPAT_FULL,
	Load_SNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SOROM
(
	"NES-SOROM",
	_T("MMC1 with 16KB SRAM"),
	COMPAT_FULL,
	Load_SOROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SUROM
(
	"NES-SUROM",
	_T("MMC1 with 512KB PRG"),
	COMPAT_FULL,
	Load_SUROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_SXROM
(
	"NES-SXROM",
	_T("MMC1 with 512KB PRG and 32KB SRAM"),
	COMPAT_FULL,
	Load_SXROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
);
