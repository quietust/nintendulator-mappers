#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC4.h"

static	void	Sync (void)
{
	MMC4_SyncPRG(0xF,0);
	MMC4_SyncCHR();
	MMC4_SyncMirror();
	EMU->SetPRG_RAM8(0x6,0);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC4_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();
	MMC4_Init(ResetType,Sync);
}

static	u8 MapperNum = 10;
CTMapperInfo	MapperInfo_010 =
{
	&MapperNum,
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