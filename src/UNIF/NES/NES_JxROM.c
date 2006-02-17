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

static	void	MAPINT	Load_JLROM (void)
{
	FME7_Load(Sync_JLROM);
}
static	void	MAPINT	Load_JSROM (void)
{
	FME7_Load(Sync_JSROM);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	FME7_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	FME7_Unload();
}

CTMapperInfo	MapperInfo_NES_JLROM =
{
	"NES-JLROM",
	_T("Sunsoft FME-7"),
	COMPAT_FULL,
	Load_JLROM,
	Reset,
	Unload,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};
CTMapperInfo	MapperInfo_NES_JSROM =
{
	"NES-JSROM",
	_T("Sunsoft FME-7 with WRAM"),
	COMPAT_FULL,
	Load_JSROM,
	Reset,
	Unload,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};CTMapperInfo	MapperInfo_NES_BTR =
{
	"NES-BTR",
	_T("Sunsoft FME-7 with WRAM (NES-JSROM)"),
	COMPAT_FULL,
	Load_JSROM,
	Reset,
	Unload,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};