/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

namespace
{
void	Sync_TBROM (void)
{
	MMC3::SyncPRG(0x07, 0);
	MMC3::SyncCHR_ROM(0x3F, 0);
	MMC3::SyncMirror();
}
void	Sync_TEROM (void)
{
	MMC3::SyncPRG(0x03, 0);
	MMC3::SyncCHR_ROM(0x3F, 0);
	UNIF_SetMirroring(MMC3::SyncMirror);
}
void	Sync_TFROM (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0x3F, 0);
	UNIF_SetMirroring(MMC3::SyncMirror);
}
void	Sync_TGROM (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_RAM(0x7, 0);
	MMC3::SyncMirror();
}
void	Sync_TKROM (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
}
void	Sync_TKSROM (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0x7F, 0);
	EMU->SetCHR_NT1(0x8, (MMC3::GetCHRBank(0) & 0x80) >> 7);
	EMU->SetCHR_NT1(0x9, (MMC3::GetCHRBank(1) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xA, (MMC3::GetCHRBank(2) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xB, (MMC3::GetCHRBank(3) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xC, (MMC3::GetCHRBank(4) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xD, (MMC3::GetCHRBank(5) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xE, (MMC3::GetCHRBank(6) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xF, (MMC3::GetCHRBank(7) & 0x80) >> 7);
}
void	Sync_TLROM (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
}
void	Sync_TLSROM (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0x7F, 0);
	EMU->SetCHR_NT1(0x8, (MMC3::GetCHRBank(0) & 0x80) >> 7);
	EMU->SetCHR_NT1(0x9, (MMC3::GetCHRBank(1) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xA, (MMC3::GetCHRBank(2) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xB, (MMC3::GetCHRBank(3) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xC, (MMC3::GetCHRBank(4) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xD, (MMC3::GetCHRBank(5) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xE, (MMC3::GetCHRBank(6) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xF, (MMC3::GetCHRBank(7) & 0x80) >> 7);
}
void	Sync_TNROM (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_RAM(0x7, 0);
	MMC3::SyncMirror();
}
void	Sync_TQROM (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x0F, 0);

	for (int i = 0; i < 8; i++)
	{
		uint8_t bank = MMC3::GetCHRBank(i);
		if (bank & 0x40)
			EMU->SetCHR_RAM1(i, bank & 0x07);
		else	EMU->SetCHR_ROM1(i, bank & 0x3F);
	}
	MMC3::SyncMirror();
}
void	Sync_TR1ROM (void)
{
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0x3F, 0);
	EMU->Mirror_4S();
}
void	Sync_TSROM (void)
{
	MMC3::SyncWRAM();
	MMC3::SyncPRG(0x3F, 0);
	MMC3::SyncCHR_ROM(0xFF, 0);
	MMC3::SyncMirror();
}
void	Sync_TVROM (void)
{
	MMC3::SyncPRG(0x07, 0);
	MMC3::SyncCHR_ROM(0x3F, 0);
	EMU->Mirror_4S();
}

BOOL	MAPINT	Load_TBROM (void)
{
	MMC3::Load(Sync_TBROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TEROM (void)
{
	MMC3::Load(Sync_TEROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TFROM (void)
{
	MMC3::Load(Sync_TFROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TGROM (void)
{
	MMC3::Load(Sync_TGROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC3::Load(Sync_TKROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TKSROM (void)
{
	UNIF_SetSRAM(8192);
	MMC3::Load(Sync_TKSROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TLROM (void)
{
	MMC3::Load(Sync_TLROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TLSROM (void)
{
	MMC3::Load(Sync_TLSROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TNROM (void)
{
	UNIF_SetSRAM(8192);
	MMC3::Load(Sync_TNROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TQROM (void)
{
	MMC3::Load(Sync_TQROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TR1ROM (void)
{
	MMC3::Load(Sync_TR1ROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TSROM (void)
{
	MMC3::Load(Sync_TSROM, TRUE);
	return TRUE;
}
BOOL	MAPINT	Load_TVROM (void)
{
	MMC3::Load(Sync_TVROM, TRUE);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_TBROM
(
	"NES-TBROM",
	_T("MMC3 with small PRG-ROM"),
	COMPAT_FULL,
	Load_TBROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TEROM
(
	"NES-TEROM",
	_T("MMC3 with small PRG-ROM and optional hardwired mirroring"),
	COMPAT_FULL,
	Load_TEROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TFROM
(
	"NES-TFROM",
	_T("MMC3 with optional hardwired mirroring"),
	COMPAT_FULL,
	Load_TFROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TGROM
(
	"NES-TGROM",
	_T("MMC3 with CHR-RAM"),
	COMPAT_FULL,
	Load_TGROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TKROM
(
	"NES-TKROM",
	_T("MMC3 with CHR-ROM and SRAM"),
	COMPAT_FULL,
	Load_TKROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TKSROM
(
	"NES-TKSROM",
	_T("MMC3 with CHR-ROM and SRAM and custom mirroring"),
	COMPAT_FULL,
	Load_TKSROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TLROM
(
	"NES-TLROM",
	_T("MMC3 with CHR-ROM"),
	COMPAT_FULL,
	Load_TLROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TL1ROM
(
	"NES-TL1ROM",
	_T("MMC3 with CHR-ROM"),
	COMPAT_FULL,
	Load_TLROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TL2ROM
(
	"NES-TL2ROM",
	_T("MMC3 with CHR-ROM"),
	COMPAT_FULL,
	Load_TLROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TLSROM
(
	"NES-TLSROM",
	_T("MMC3 with CHR-ROM and custom mirroring"),
	COMPAT_FULL,
	Load_TLSROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TNROM
(
	"NES-TNROM",
	_T("MMC3 with CHR-RAM and SRAM"),
	COMPAT_FULL,
	Load_TNROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TQROM
(
	"NES-TQROM",
	_T("MMC3 with CHR-ROM and CHR-RAM"),
	COMPAT_FULL,
	Load_TQROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TR1ROM
(
	"NES-TR1ROM",
	_T("MMC3 with 4-screen VRAM"),
	COMPAT_FULL,
	Load_TR1ROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TSROM
(
	"NES-TSROM",
	_T("MMC3 with WRAM"),
	COMPAT_FULL,
	Load_TSROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
const MapperInfo MapperInfo_NES_TVROM
(
	"NES-TVROM",
	_T("MMC3 with small PRG ROM and 4-screen VRAM"),
	COMPAT_FULL,
	Load_TVROM,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	MMC3::SaveLoad,
	NULL,
	NULL
);
