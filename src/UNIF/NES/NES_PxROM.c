#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC2.h"

static void Sync (void)
{
	MMC2_SyncPRG(0xF);
	MMC2_SyncCHR();
	MMC2_SyncMirror();
}

static void Reset_PNROM (const PMapperParam _MP, int IsHardReset)
{
	MP = _MP;
	MMC2_Init(Sync);
}

TMapperInfo	MapperInfo_NES_PNROM =
{
	"NES-PNROM",
	-1,
	MS_Full,
	8192,
	Reset_PNROM,
	NULL,
	NULL,
	MMC2_TileHandler,
	MMC2_SaveState,
	MMC2_LoadState,
	NULL,
	NULL
};