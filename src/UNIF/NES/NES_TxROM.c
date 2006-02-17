#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	void	Sync_TFROM (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0x3F,0);
	UNIF_SetMirroring(MMC3_SyncMirror);
}
static	void	Sync_TGROM (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_RAM(0x7,0);
	MMC3_SyncMirror();
}
static	void	Sync_TKROM (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
}
static	void	Sync_TLROM (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
}
static	void	Sync_TR1ROM (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0x3F,0);
	EMU->Mirror_4();
}
static	void	Sync_TSROM (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	MMC3_Destroy();
}

static	void	_MAPINT	Reset_TFROM (int IsHardReset)
{
	MMC3_Init(Sync_TFROM);
}
static	void	_MAPINT	Reset_TGROM (int IsHardReset)
{
	MMC3_Init(Sync_TGROM);
}
static	void	_MAPINT	Reset_TKROM (int IsHardReset)
{
	UNIF_InitSRAM(8192);
	MMC3_Init(Sync_TKROM);
}
static	void	_MAPINT	Reset_TLROM (int IsHardReset)
{
	MMC3_Init(Sync_TLROM);
}
static	void	_MAPINT	Reset_TR1ROM (int IsHardReset)
{
	MMC3_Init(Sync_TR1ROM);
}
static	void	_MAPINT	Reset_TSROM (int IsHardReset)
{
	MMC3_Init(Sync_TSROM);
}

CTMapperInfo	MapperInfo_NES_TFROM =
{
	"NES-TFROM",
	"NES-TFROM",
	COMPAT_NEARLY,
	Reset_TFROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_TGROM =
{
	"NES-TGROM",
	"NES-TGROM",
	COMPAT_NEARLY,
	Reset_TGROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_TKROM =
{
	"NES-TKROM",
	"NES-TKROM",
	COMPAT_NEARLY,
	Reset_TKROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_TLROM =
{
	"NES-TLROM",
	"NES-TLROM",
	COMPAT_NEARLY,
	Reset_TLROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_TR1ROM =
{
	"NES-TR1ROM",
	"NES-TR1ROM",
	COMPAT_NEARLY,
	Reset_TR1ROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_TSROM =
{
	"NES-TSROM",
	"NES-TSROM",
	COMPAT_NEARLY,
	Reset_TSROM,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};