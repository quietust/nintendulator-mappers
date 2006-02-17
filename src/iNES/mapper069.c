#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_FME7.h"

static	void	Sync (void)
{
	FME7_SyncPRG(0xFF,0);
	FME7_SyncCHR(0xFF,0);
	FME7_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	FME7_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	FME7_Init(ResetType,Sync);
}

static	u8 MapperNum = 69;
CTMapperInfo	MapperInfo_069 =
{
	&MapperNum,
	"Sunsoft FME-7",
	COMPAT_FULL,
	Reset,
	Shutdown,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};