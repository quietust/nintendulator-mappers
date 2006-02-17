#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_GNROM (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x3);
	EMU->SetCHR_ROM8(0,(Latch.Data >> 0) & 0x3);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset_GNROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_GNROM,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_GNROM =
{
	"NES-GNROM",
	"Standard 32KB PRG/8KB CHR switch",
	COMPAT_FULL,
	Reset_GNROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};