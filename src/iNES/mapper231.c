#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	if (Latch.Addr.b0 & 0x20)
		EMU->SetPRG_ROM32(0x8,(Latch.Addr.b0 & 0x1E) >> 1);
	else
	{
		EMU->SetPRG_ROM16(0x8,Latch.Addr.b0 & 0x1E);
		EMU->SetPRG_ROM16(0xC,Latch.Addr.b0 & 0x1E);
	}
	switch ((Latch.Addr.b0 & 0xC0) >> 6)
	{
	case 0:	EMU->Mirror_S0();		break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_H();		break;
	case 3:	EMU->Mirror_Custom(0,1,1,1);	break;
	}
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

static	u8 MapperNum = 231;
CTMapperInfo	MapperInfo_231 =
{
	&MapperNum,
	"20-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AL,
	NULL,
	NULL
};