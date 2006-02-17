#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_CNROM (void)
{
	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,1);
	EMU->SetCHR_ROM8(0,Latch.Data & 0x03);
}
static	void	Sync_CPROM (void)
{
	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,1);
	EMU->SetCHR_RAM4(0,0);
	EMU->SetCHR_RAM4(4,Latch.Data & 0x3);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset_CNROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_CNROM,TRUE);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Reset_CPROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_CPROM,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_CNROM =
{
	"NES-CNROM",
	"Standard 8KB CHR switch",
	COMPAT_FULL,
	Reset_CNROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_CPROM =
{
	"NES-CPROM",
	"4KB CHR RAM switch",
	COMPAT_FULL,
	Reset_CPROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};