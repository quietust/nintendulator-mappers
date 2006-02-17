#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC6.h"

static	void	Sync_HKROM (void)
{
	MMC6_SyncPRG(0x3F,0);
	MMC6_SyncCHR_ROM(0xFF,0);
	MMC6_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC6_Destroy();
	UNIF_SaveSRAM();
}

static	void	_MAPINT	Reset_HKROM (RESET_TYPE ResetType)
{
	UNIF_InitSRAM(1024);
	MMC6_Init(ResetType,Sync_HKROM);
}

CTMapperInfo	MapperInfo_NES_HKROM =
{
	"NES-HKROM",
	"MMC6",
	COMPAT_FULL,
	Reset_HKROM,
	Shutdown,
	NULL,
	MMC6_PPUCycle,
	MMC6_SaveLoad,
	NULL,
	NULL
};