#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRG     : 4;
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned         : 1;
			unsigned Mir_HV  : 1;
			unsigned         : 8;
		};
		struct
		{
			unsigned n       :16;
		};
	}	Addr;
	Addr.n = Latch.Addr;
	EMU->SetCHR_RAM8(0,0);
	if (Addr.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(Addr.PRG << 1) | Addr.PRG16);
		EMU->SetPRG_ROM16(0xC,(Addr.PRG << 1) | Addr.PRG16);
	}
	else	EMU->SetPRG_ROM32(0x8,Addr.PRG);
	if (Addr.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
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

static	u8 MapperNum = 61;
CTMapperInfo	MapperInfo_061 =
{
	&MapperNum,
	"20-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};