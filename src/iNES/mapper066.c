#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetCHR_ROM8 (  0,(Latch.Data >> 0) & 0x3);
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x3);
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

static	u8 MapperNum = 66;
CTMapperInfo	MapperInfo_066 =
{
	&MapperNum,
	"GNROM",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};