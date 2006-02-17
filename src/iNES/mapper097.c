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
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_V();	break;
	case 3:	/* EMU->Mirror_S1();
		/* uncertain */		break;
	}
}

static	void	_MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 97;
CTMapperInfo	MapperInfo_097 =
{
	&MapperNum,
	_T("Kid Niki (J)"),
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