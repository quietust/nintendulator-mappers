#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync_AMROM (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x3);
	EMU->SetCHR_RAM8(0,0);
	if (Latch.Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}
static	void	Sync_ANROM (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x3);
	EMU->SetCHR_RAM8(0,0);
	if (Latch.Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}
static	void	Sync_AOROM (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x7);
	EMU->SetCHR_RAM8(0,0);
	if (Latch.Data & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

static	void	_MAPINT	Reset_AMROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_AMROM,TRUE);
}
static	void	_MAPINT	Reset_ANROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_ANROM,FALSE);
}
static	void	_MAPINT	Reset_AOROM (RESET_TYPE ResetType)
{
	Latch_Init(ResetType,Sync_AOROM,TRUE);
}

CTMapperInfo	MapperInfo_NES_AMROM =
{
	"NES-AMROM",
	"32KB PRG switch with nametable select",
	COMPAT_FULL,
	Reset_AMROM,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_ANROM =
{
	"NES-ANROM",
	"Standard 32KB PRG switch with nametable select",
	COMPAT_FULL,
	Reset_ANROM,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_AOROM =
{
	"NES-AOROM",
	"Oversize 32KB PRG switch with nametable select",
	COMPAT_FULL,
	Reset_AOROM,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};