#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_FME7.h"

static	void	Sync_JLROM (void)
{
	FME7_SyncPRG(0x3F,0);
	FME7_SyncCHR(0xFF,0);
	FME7_SyncMirror();
}

static	void	Sync_BTR (void)
{
	FME7_SyncPRG(0x3F,0);
	FME7_SyncCHR(0xFF,0);
	FME7_SyncMirror();
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	FME7_Destroy();
}

static	void	_MAPINT	Reset_JLROM (int IsHardReset)
{
	FME7_Init(Sync_JLROM);
}
static	void	_MAPINT	Reset_BTR (int IsHardReset)
{
	FME7_Init(Sync_BTR);
}

CTMapperInfo	MapperInfo_NES_JLROM =
{
	"NES-JLROM",
	"Sunsoft FME-7",
	COMPAT_FULL,
	Reset_JLROM,
	Shutdown,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};
CTMapperInfo	MapperInfo_NES_BTR =
{
	"NES-BTR",
	"Sunsoft FME-7 with WRAM",
	COMPAT_FULL,
	Reset_BTR,
	Shutdown,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};