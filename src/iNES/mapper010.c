#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC4.h"

static	void	Sync (void)
{
	MMC4_SyncPRG(0xFF,0);
	MMC4_SyncCHR();
	MMC4_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC4_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();
	MMC4_Init(Sync);
}

CTMapperInfo	MapperInfo_010 =
{
	10,
	NULL,
	"MMC4",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	MMC4_SaveLoad,
	NULL,
	NULL
};