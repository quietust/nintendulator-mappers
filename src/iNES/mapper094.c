#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data >> 2) & 0x7);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	Latch_Init(ResetType,Sync,FALSE);
}

static	u8 MapperNum = 94;
CTMapperInfo	MapperInfo_094 =
{
	&MapperNum,
	"Mapper 94",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};