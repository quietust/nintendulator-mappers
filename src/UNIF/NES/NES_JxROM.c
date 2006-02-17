#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_FME7.h"

static	void	Sync_JLROM (void)
{
	FME7_SyncPRG(0x3F,0);
	FME7_SyncCHR(0xFF,0);
	FME7_SyncMirror();
}

static	void	Sync_JSROM (void)
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

static	void	_MAPINT	Reset_JLROM (RESET_TYPE ResetType)
{
	FME7_Init(ResetType,Sync_JLROM);
}
static	void	_MAPINT	Reset_JSROM (RESET_TYPE ResetType)
{
	FME7_Init(ResetType,Sync_JSROM);
}
static	void	_MAPINT	Reset_BTR (RESET_TYPE ResetType)
{
	FME7_Init(ResetType,Sync_JSROM);
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
CTMapperInfo	MapperInfo_NES_JSROM =
{
	"NES-JSROM",
	"Sunsoft FME-7 with WRAM",
	COMPAT_FULL,
	Reset_BTR,
	Shutdown,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};CTMapperInfo	MapperInfo_NES_BTR =
{
	"NES-BTR",
	"Sunsoft FME-7 with WRAM (NES-JSROM)",
	COMPAT_FULL,
	Reset_BTR,
	Shutdown,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};