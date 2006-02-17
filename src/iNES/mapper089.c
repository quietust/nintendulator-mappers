#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data & 0x70) >> 4);
	EMU->SetPRG_ROM16(0xC,0x7);
	EMU->SetCHR_ROM8(0,(Latch.Data & 0x7) | ((Latch.Data & 0x80) >> 4));
	if (Latch.Data & 0x08)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 89;
CTMapperInfo	MapperInfo_089 =
{
	&MapperNum,
	_T("Sunsoft 74161/7432"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};