#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC4.h"

static	void	Sync_FJROM (void)
{
	MMC4_SyncPRG(0xF,0);
	MMC4_SyncCHR();
	MMC4_SyncMirror();
	EMU->SetPRG_RAM8(0x6,0);
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	MMC4_Destroy();
}

static	void	_MAPINT	Reset_FJROM (int IsHardReset)
{
	MMC4_Init(Sync_FJROM);
	UNIF_InitSRAM(8192);
}

CTMapperInfo	MapperInfo_NES_FJROM =
{
	"NES-FJROM",
	"MMC4",
	COMPAT_FULL,
	Reset_FJROM,
	Shutdown,
	NULL,
	NULL,
	MMC4_SaveLoad,
	NULL,
	NULL
};
