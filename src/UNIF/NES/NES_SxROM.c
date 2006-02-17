#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC1.h"

static	void	Sync_SAROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0x7,0);
	MMC1_SyncCHR_ROM(0x0F,0);
	MMC1_SyncWRAM();
}
static	void	Sync_SBROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0x7,0);
	MMC1_SyncCHR_ROM(0x0F,0);
}
static	void	Sync_SCROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0x7,0);
	MMC1_SyncCHR_ROM(0x1F,0);
}
static	void	Sync_SEROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0x1,0);
	MMC1_SyncCHR_ROM(0x0F,0);
}
static	void	Sync_SGROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	MMC1_SyncCHR_RAM(0x1,0);
}
static	void	Sync_SKROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	MMC1_SyncCHR_ROM(0x1F,0);
	MMC1_SyncWRAM();
}
static	void	Sync_SLROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	MMC1_SyncCHR_ROM(0x1F,0);
}
static	void	Sync_SL1ROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0x7,0);
	MMC1_SyncCHR_ROM(0x1F,0);
}
static	void	Sync_SNROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	MMC1_SyncCHR_RAM(0x01,0);
	MMC1_SyncWRAM();
}
static	void	Sync_SOROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,0);
	MMC1_SyncCHR_RAM(0x01,0);
	if (MMC1_GetWRAMEnabled())
		EMU->SetPRG_RAM8(0x6,((MMC1_GetCHRBankLo() & 0x10) >> 4) ^ 1);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}
static	void	Sync_SUROM (void)
{
	MMC1_SyncMirror();
	MMC1_SyncPRG(0xF,MMC1_GetCHRBankLo() & 0x10);
	MMC1_SyncCHR_RAM(0x01,0);
	MMC1_SyncWRAM();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC1_Destroy();
}

static	void	_MAPINT	Reset_SAROM (RESET_TYPE ResetType)
{
	UNIF_SetSRAM(8192);
	MMC1_Init(ResetType,Sync_SAROM);
}
static	void	_MAPINT	Reset_SBROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SBROM);
}
static	void	_MAPINT	Reset_SCROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SCROM);
}
static	void	_MAPINT	Reset_SEROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SEROM);
}
static	void	_MAPINT	Reset_SGROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SGROM);
}
static	void	_MAPINT	Reset_SKROM (RESET_TYPE ResetType)
{
	UNIF_SetSRAM(8192);
	MMC1_Init(ResetType,Sync_SKROM);
}
static	void	_MAPINT	Reset_SLROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SLROM);
}
static	void	_MAPINT	Reset_SL1ROM (RESET_TYPE ResetType)
{
	MMC1_Init(ResetType,Sync_SL1ROM);
}
static	void	_MAPINT	Reset_SNROM (RESET_TYPE ResetType)
{
	UNIF_SetSRAM(8192);
	MMC1_Init(ResetType,Sync_SNROM);
}
static	void	_MAPINT	Reset_SOROM (RESET_TYPE ResetType)
{
	UNIF_SetSRAM(8192);
	MMC1_Init(ResetType,Sync_SOROM);
}
static	void	_MAPINT	Reset_SUROM (RESET_TYPE ResetType)
{
	UNIF_SetSRAM(8192);
	MMC1_Init(ResetType,Sync_SUROM);
}

CTMapperInfo	MapperInfo_NES_SAROM =
{
	"NES-SAROM",
	"MMC1 with SRAM",
	COMPAT_FULL,
	Reset_SAROM,
	Shutdown,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SBROM =
{
	"NES-SBROM",
	"MMC1",
	COMPAT_FULL,
	Reset_SBROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SCROM =
{
	"NES-SCROM",
	"MMC1",
	COMPAT_FULL,
	Reset_SCROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SEROM =
{
	"NES-SEROM",
	"MMC1",
	COMPAT_FULL,
	Reset_SEROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SGROM =
{
	"NES-SGROM",
	"MMC1 with CHR-RAM",
	COMPAT_FULL,
	Reset_SGROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SKROM =
{
	"NES-SKROM",
	"MMC1 with CHR-ROM and SRAM",
	COMPAT_FULL,
	Reset_SKROM,
	Shutdown,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SLROM =
{
	"NES-SLROM",
	"MMC1 with CHR-ROM",
	COMPAT_FULL,
	Reset_SLROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SL1ROM =
{
	"NES-SL1ROM",
	"MMC1",
	COMPAT_FULL,
	Reset_SL1ROM,
	NULL,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SNROM =
{
	"NES-SNROM",
	"MMC1 with CHR-RAM and SRAM",
	COMPAT_FULL,
	Reset_SNROM,
	Shutdown,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SOROM =
{
	"NES-SOROM",
	"MMC1 with 16KB SRAM",
	COMPAT_FULL,
	Reset_SOROM,
	Shutdown,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_SUROM =
{
	"NES-SUROM",
	"MMC1 with 512KB PRG",
	COMPAT_FULL,
	Reset_SNROM,
	Shutdown,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};