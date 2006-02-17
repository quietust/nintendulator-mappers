#include "..\..\DLL\d_UNIF.h"
#include "..\..\Hardware\h_MMC4.h"

static	void	Sync_FJROM (void)
{
	MMC4_SyncPRG(0xF,0);
	MMC4_SyncCHR();
	MMC4_SyncMirror();
	EMU->SetPRG_RAM8(0x6,0);
}

static	void	_MAPINT	Load_FJROM (void)
{
	UNIF_SetSRAM(8192);
	MMC4_Load(Sync_FJROM);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC4_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC4_Unload();
}


CTMapperInfo	MapperInfo_NES_FJROM =
{
	"NES-FJROM",
	"MMC4",
	COMPAT_FULL,
	Load_FJROM,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC4_SaveLoad,
	NULL,
	NULL
};
