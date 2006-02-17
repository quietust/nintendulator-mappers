#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_FME7.h"

static	void	Sync (void)
{
	FME7_SyncPRG(0xFF,0);
	FME7_SyncCHR(0xFF,0);
	FME7_SyncMirror();
}

static	void	_MAPINT	Load (void)
{
	FME7_Load(Sync);
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	FME7_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	FME7_Unload();
}

static	u8 MapperNum = 69;
CTMapperInfo	MapperInfo_069 =
{
	&MapperNum,
	_T("Sunsoft FME-7"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	FME7_CPUCycle,
	NULL,
	FME7_SaveLoad,
	FME7_GenSound,
	NULL
};