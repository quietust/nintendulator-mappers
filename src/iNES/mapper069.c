#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_FME07.h"

static	void	Sync (void)
{
	FME07_SyncPRG(0xFF,0);
	FME07_SyncCHR(0xFF,0);
	FME07_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	FME07_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	FME07_Init(Sync);
}

CTMapperInfo	MapperInfo_069 =
{
	69,
	NULL,
	"Sunsoft FME-07",
	COMPAT_FULL,
	Reset,
	Shutdown,
	FME07_CPUCycle,
	NULL,
	FME07_SaveLoad,
	FME07_GenSound,
	NULL
};