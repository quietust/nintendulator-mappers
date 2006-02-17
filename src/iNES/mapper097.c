#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,-1);
	EMU->SetPRG_ROM16(0xC,Latch.Data & 0xF);
	EMU->SetCHR_RAM8(0,0);
	switch ((Latch.Data >> 6) & 0x3)
	{
	case 0:	/* EMU->Mirror_S0();
		/* uncertain */		break;
	case 1:	EMU->Mirror_H();		break;
	case 2:	EMU->Mirror_V();		break;
	case 3:	/* EMU->Mirror_S1();
		/* uncertain */		break;
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

CTMapperInfo	MapperInfo_097 =
{
	97,
	NULL,
	"Kid Niki (J)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
