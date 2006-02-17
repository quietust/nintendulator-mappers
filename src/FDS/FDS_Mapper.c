#include	"..\DLL\d_FDS.h"
#include	"..\Hardware\h_FDS.h"

static	void	MAPINT	Load (void)
{
	FDS_Load();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	FDS_Reset(ResetType);
}

static	void	MAPINT	Unload (void)
{
	FDS_Unload();
}

CTMapperInfo	MapperInfo_FDS =
{
	NULL,
	_T("Famicom Disk System"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	FDS_CPUCycle,
	NULL,
	FDS_SaveLoad,
	FDS_MapperSnd,
	FDS_Config
};