#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data & 0x70) >> 4);
	EMU->SetPRG_ROM16(0xC,0x7);
	EMU->SetCHR_ROM8(0,Latch.Data & 0xF);
	if (Latch.Data & 0x80)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	Latch_Init(ResetType,Sync,FALSE);
}

static	u8 MapperNum = 70;
CTMapperInfo	MapperInfo_070 =
{
	&MapperNum,
	"Bandai 74161/7432",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};