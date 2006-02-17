#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,(Latch.Data & 0x80) >> 7);
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

static	u8 MapperNum = 149;
CTMapperInfo	MapperInfo_149 =
{
	&MapperNum,
	"Sachen (SA-0036)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
