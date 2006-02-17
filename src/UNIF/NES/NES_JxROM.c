#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_FME07.h"

static	void	Sync_JLROM (void)
{
	FME07_SyncPRG(0x3F,0);
	FME07_SyncCHR(0xFF,0);
	FME07_SyncMirror();
}

static	void	Sync_BTR (void)
{
	FME07_SyncPRG(0x3F,0);
	FME07_SyncCHR(0xFF,0);
	FME07_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	FME07_Destroy();
}

static	void	_MAPINT	Reset_JLROM (int IsHardReset)
{
	FME07_Init(Sync_JLROM);
}
static	void	_MAPINT	Reset_BTR (int IsHardReset)
{
	FME07_Init(Sync_BTR);
}

CTMapperInfo	MapperInfo_NES_JLROM =
{
	"NES-JLROM",
	"NES-JLROM",
	COMPAT_FULL,
	Reset_JLROM,
	Shutdown,
	FME07_CPUCycle,
	NULL,
	FME07_SaveLoad,
	FME07_GenSound,
	NULL
};
CTMapperInfo	MapperInfo_NES_BTR =
{
	"NES-BTR",
	"NES-BTR",
	COMPAT_FULL,
	Reset_BTR,
	Shutdown,
	FME07_CPUCycle,
	NULL,
	FME07_SaveLoad,
	FME07_GenSound,
	NULL
};