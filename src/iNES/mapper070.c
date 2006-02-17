#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data & 0x70) >> 4);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM8(0,Latch.Data & 0x0F);
	if (Latch.Data & 0x80)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Latch_Init(Sync,IsHardReset,FALSE);
}

CTMapperInfo	MapperInfo_070 =
{
	70,
	NULL,
	"Mapper 70",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};