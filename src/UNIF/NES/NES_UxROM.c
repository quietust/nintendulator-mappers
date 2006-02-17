#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_UNROM (void)
{
	EMU->SetPRG_ROM16(0x8,Latch.Data & 0x7);
	EMU->SetPRG_ROM16(0xC,0x7);
	EMU->SetCHR_RAM8(0,0);
}
static	void	Sync_UOROM (void)
{
	EMU->SetPRG_ROM16(0x8,Latch.Data & 0xF);
	EMU->SetPRG_ROM16(0xC,0xF);
	EMU->SetCHR_RAM8(0,0);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset_UNROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_UNROM,TRUE);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Reset_UOROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_UOROM,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_UNROM =
{
	"NES-UNROM",
	"Standard 16KB PRG switch",
	COMPAT_FULL,
	Reset_UNROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_UOROM =
{
	"NES-UOROM",
	"Oversize 16KB PRG switch",
	COMPAT_FULL,
	Reset_UOROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};