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

static	void	MAPINT	Load_AMROM (void)
{
	Latch_Load(Sync_AMROM,TRUE);
}
static	void	MAPINT	Load_ANROM (void)
{
	Latch_Load(Sync_ANROM,FALSE);
}
static	void	MAPINT	Load_AOROM (void)
{
	Latch_Load(Sync_AOROM,TRUE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

CTMapperInfo	MapperInfo_NES_AMROM =
{
	"NES-AMROM",
	_T("32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_AMROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_ANROM =
{
	"NES-ANROM",
	_T("Standard 32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_ANROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_AOROM =
{
	"NES-AOROM",
	_T("Oversize 32KB PRG switch with nametable select"),
	COMPAT_FULL,
	Load_AOROM,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};