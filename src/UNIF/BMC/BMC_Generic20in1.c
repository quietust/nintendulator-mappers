#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned         : 1;
			unsigned PRG     : 4;
			unsigned PRGsize : 1;
			unsigned Mirror  : 2;
			unsigned         : 8;
		};
		struct
		{
			unsigned addr    :16;
		};
	}	Addr;
	Addr.addr = Latch.Addr;
	
	EMU->SetCHR_RAM8(0,0);
	if (Addr.PRGsize)
		EMU->SetPRG_ROM32(0x8,Addr.PRG);
	else
	{
		EMU->SetPRG_ROM16(0x8,Addr.PRG << 1);
		EMU->SetPRG_ROM16(0x8,Addr.PRG << 1);
	}
	switch (Addr.Mirror)
	{
	case 0:	EMU->Mirror_S0();		break;
	case 1:	EMU->Mirror_V();			break;
	case 2:	EMU->Mirror_H();			break;
	case 3:	EMU->Mirror_Custom(0,1,1,1);	break;
	}
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	Latch_Init(Sync,IsHardReset,FALSE);
}

CTMapperInfo	MapperInfo_BMC_Generic20in1 =
{
	-1,
	"BMC-Generic20in1",
	"BMC-Generic20in1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};