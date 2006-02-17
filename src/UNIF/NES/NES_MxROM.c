#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_MHROM (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x1);
	EMU->SetCHR_ROM8(0,(Latch.Data >> 0) & 0x1);
}

static	void	_MAPINT	Load_MHROM (void)
{
	Latch_Load(Sync_MHROM,TRUE);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Unload (void)
{
	Latch_Unload();
}

CTMapperInfo	MapperInfo_NES_MHROM =
{
	"NES-MHROM",
	_T("Super Mario Bros/Duck Hunt"),
	COMPAT_FULL,
	Load_MHROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};