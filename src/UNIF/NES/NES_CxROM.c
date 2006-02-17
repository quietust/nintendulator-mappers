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

static	void	_MAPINT	Reset_CNROM (int IsHardReset)
{
	Latch_Init(Sync_CNROM,IsHardReset,TRUE);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Reset_CPROM (int IsHardReset)
{
	Latch_Init(Sync_CPROM,IsHardReset,TRUE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_NES_CNROM =
{
	-1,
	"NES-CNROM",
	"NES-CNROM",
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
	-1,
	"NES-CPROM",
	"NES-CPROM",
	COMPAT_FULL,
	Reset_CPROM,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};