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

static	void	_MAPINT	Load_CNROM (void)
{
	Latch_Load(Sync_CNROM,TRUE);
}
static	void	_MAPINT	Load_CPROM (void)
{
	Latch_Load(Sync_CPROM,TRUE);
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


CTMapperInfo	MapperInfo_NES_CNROM =
{
	"NES-CNROM",
	_T("Standard 8KB CHR switch"),
	COMPAT_FULL,
	Load_CNROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_CPROM =
{
	"NES-CPROM",
	_T("4KB CHR RAM switch"),
	COMPAT_FULL,
	Load_CPROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};