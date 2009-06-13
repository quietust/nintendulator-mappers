/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
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
void	Sync_SL1ROM (void)
{
	MMC1::SyncMirror();
	MMC1::SyncPRG(0x7, 0);
	MMC1::SyncCHR_ROM(0x1F, 0);
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

void	MAPINT	Load_SAROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SAROM);
}
void	MAPINT	Load_SBROM (void)
{
	MMC1::Load(Sync_SBROM);
}
void	MAPINT	Load_SCROM (void)
{
	MMC1::Load(Sync_SCROM);
}
void	MAPINT	Load_SEROM (void)
{
	MMC1::Load(Sync_SEROM);
}
void	MAPINT	Load_SFROM (void)
{
	MMC1::Load(Sync_SFROM);
}
void	MAPINT	Load_SGROM (void)
{
	MMC1::Load(Sync_SGROM);
}
void	MAPINT	Load_SKROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SKROM);
}
void	MAPINT	Load_SLROM (void)
{
	MMC1::Load(Sync_SLROM);
}
void	MAPINT	Load_SL1ROM (void)
{
	MMC1::Load(Sync_SL1ROM);
}
void	MAPINT	Load_SNROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SNROM);
}
void	MAPINT	Load_SOROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SOROM);
}
void	MAPINT	Load_SUROM (void)
{
	UNIF_SetSRAM(8192);
	MMC1::Load(Sync_SUROM);
}
void	MAPINT	Load_SXROM (void)
{
	UNIF_SetSRAM(32768);
	MMC1::Load(Sync_SXROM);
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

CTMapperInfo	MapperInfo_NES_SAROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SBROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SCROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SEROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SFROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SGROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SKROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SLROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SL1ROM =
{
	"NES-SL1ROM",
	_T("MMC1"),
	COMPAT_FULL,
	Load_SL1ROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1::SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SNROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SOROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SUROM =
{
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
};
CTMapperInfo	MapperInfo_NES_SXROM =
{
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
};