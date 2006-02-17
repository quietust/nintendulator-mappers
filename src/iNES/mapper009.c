#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC2.h"

static	void	Sync (void)
{
	MMC2_SyncPRG(0xF,0);
	MMC2_SyncCHR();
	MMC2_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC2_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();
	MMC2_Init(ResetType,Sync);
}

static	u8 MapperNum = 9;
CTMapperInfo	MapperInfo_009 =
{
	&MapperNum,
	"MMC2",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	MMC2_SaveLoad,
	NULL,
	NULL
};