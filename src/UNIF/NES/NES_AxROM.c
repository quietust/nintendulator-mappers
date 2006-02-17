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

static	void	_MAPINT	Reset_AMROM (int IsHardReset)
{
	Latch_Init(Sync_AMROM,IsHardReset,TRUE);
}
static	void	_MAPINT	Reset_ANROM (int IsHardReset)
{
	Latch_Init(Sync_ANROM,IsHardReset,FALSE);
}
static	void	_MAPINT	Reset_AOROM (int IsHardReset)
{
	Latch_Init(Sync_AOROM,IsHardReset,TRUE);
}

CTMapperInfo	MapperInfo_NES_AMROM =
{
	"NES-AMROM",
	"NES-AMROM",
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
	"NES-ANROM",
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
	"NES-AOROM",
	COMPAT_FULL,
	Reset_AOROM,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};