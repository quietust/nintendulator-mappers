#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC2.h"

static	void	Sync_PNROM (void)
{
	MMC2_SyncPRG(0xF,0);
	MMC2_SyncCHR();
	MMC2_SyncMirror();
}

static	void	_MAPINT	Load_PNROM (void)
{
	MMC2_Load(Sync_PNROM);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC2_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC2_Unload();
}

CTMapperInfo	MapperInfo_NES_PNROM =
{
	"NES-PNROM",
	"MMC2",
	COMPAT_FULL,
	Load_PNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2_SaveLoad,
	NULL,
	NULL
};
