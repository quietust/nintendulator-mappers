#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_BNROM (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x3);
	EMU->SetCHR_RAM8(0,0);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset_BNROM (int IsHardReset)
{
	Latch_Init(Sync_BNROM,IsHardReset,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_BNROM =
{
	"NES-BNROM",
	"NES-BNROM",
	COMPAT_FULL,
	Reset_BNROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};