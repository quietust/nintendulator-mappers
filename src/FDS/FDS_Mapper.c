#include	"..\DLL\d_FDS.h"
#include	"..\Hardware\h_FDS.h"

static	void	_MAPINT	Shutdown (void)
{
	FDS_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	FDS_Init(ResetType);
}

CTMapperInfo	MapperInfo_FDS =
{
	NULL,
	"Famicom Disk System",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	FDS_CPUCycle,
	NULL,
	FDS_SaveLoad,
	FDS_MapperSnd,
	FDS_Config
};