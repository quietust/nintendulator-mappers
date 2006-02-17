#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC2.h"

static	void	Sync (void)
{
	MMC2_SyncPRG(0xF,0);
	MMC2_SyncCHR();
	MMC2_SyncMirror();
}

static	void	MAPINT	Load (void)
{
	MMC2_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	MMC2_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC2_Unload();
}

static	u8 MapperNum = 9;
CTMapperInfo	MapperInfo_009 =
{
	&MapperNum,
	_T("MMC2"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC2_SaveLoad,
	NULL,
	NULL
};