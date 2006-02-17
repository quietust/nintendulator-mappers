#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data & 0x8) >> 3);
	EMU->SetCHR_ROM8(0,Latch.Data & 0x7);
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

static	u8 MapperNum = 148;
CTMapperInfo	MapperInfo_148 =
{
	&MapperNum,
	"Sachen (SA-004/SA-0037)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
