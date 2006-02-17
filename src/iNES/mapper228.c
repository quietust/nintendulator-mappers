#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned CHRbank : 4;
			unsigned         : 1;
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 4;
			unsigned PRGchip : 2;
			unsigned Mir_HV  : 1;
			unsigned         : 2;
		};
		struct
		{
			unsigned n       :16;
		};
	}	Addr;
	u8 openbus = 0;
	Addr.n = Latch.Addr;

	if (Addr.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	
	switch (Addr.PRGchip)
	{
	case 2:	openbus = 1;		break;
	case 3:	Addr.PRGchip = 2;	break;
	}
	if (openbus)
	{
		for (openbus = 0x8; openbus <= 0xF; openbus++)
			EMU->SetPRG_OB4(openbus);
	}
	else
	{
		if (Addr.PRGsize)
		{
			EMU->SetPRG_ROM16(0x8,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) | (Addr.PRG16));
			EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) | (Addr.PRG16));
		}
		else EMU->SetPRG_ROM32(0x8,(Addr.PRGchip << 4) | (Addr.PRGbank));
	}
	EMU->SetCHR_ROM8(0,(Addr.CHRbank << 2) | (Latch.Data & 0x3));
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

CTMapperInfo	MapperInfo_228 =
{
	228,
	NULL,
	"Action 52",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};