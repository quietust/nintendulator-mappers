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

static	void	_MAPINT	Load_UNROM (void)
{
	Latch_Load(Sync_UNROM,TRUE);
}
static	void	_MAPINT	Load_UOROM (void)
{
	Latch_Load(Sync_UOROM,TRUE);
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

CTMapperInfo	MapperInfo_NES_UNROM =
{
	"NES-UNROM",
	"Standard 16KB PRG switch",
	COMPAT_FULL,
	Load_UNROM,
	Reset,
	Unload,
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
	Load_UOROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};