#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_MHROM (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x1);
	EMU->SetCHR_ROM8(0,(Latch.Data >> 0) & 0x1);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset_MHROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_MHROM,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_MHROM =
{
	"NES-MHROM",
	"Super Mario Bros/Duck Hunt",
	COMPAT_FULL,
	Reset_MHROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};