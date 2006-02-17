#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned CHRbank : 6;
			unsigned PRG16   : 1;
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_HV  : 1;
			unsigned         : 2;
		};
		struct
		{
			unsigned addr    :16;
		};
	}	Addr;
	Addr.addr = Latch.Addr;

	EMU->SetCHR_ROM8(0,Addr.CHRbank);
	if (Addr.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(Addr.PRGbank << 1) | (Addr.PRG16));
		EMU->SetPRG_ROM16(0xC,(Addr.PRGbank << 1) | (Addr.PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8,Addr.PRGbank);
	if (Addr.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	Latch_Init(Sync,IsHardReset,FALSE);
}

CTMapperInfo	MapperInfo_BMC_Generic52in1 =
{
	"BMC-Generic52in1",
	"BMC-Generic52in1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};