#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM32(0x8,0);
	if (Latch.Data & 0x02)
		EMU->SetCHR_ROM8(0,Latch.Data & 1);
	else
	{
		for (x = 0; x < 8; x++)
			EMU->SetCHR_OB1(x);
	}
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

static	u8 MapperNum = 185;
CTMapperInfo	MapperInfo_185 =
{
	&MapperNum,
	"CNROM with CHR disable",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
