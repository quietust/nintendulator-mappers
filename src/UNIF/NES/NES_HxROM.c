#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC6.h"

static	void	Sync_HKROM (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	MMC6_SyncPRG(0x3F,0);
	MMC6_SyncCHR_ROM(0xFF,0);
	MMC6_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC6_Destroy();
	UNIF_SaveSRAM();
}

static	void	_MAPINT	Reset_HKROM (int IsHardReset)
{
	MMC6_Init(Sync_HKROM);
	UNIF_InitSRAM(1024);
}

CTMapperInfo	MapperInfo_NES_HKROM =
{
	"NES-HKROM",
	"NES-HKROM",
	COMPAT_NEARLY,
	Reset_HKROM,
	Shutdown,
	NULL,
	MMC6_PPUCycle,
	MMC6_SaveLoad,
	NULL,
	NULL
};