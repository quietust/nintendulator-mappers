#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned CHRhi   : 5;
			unsigned PRGsize : 1;
			unsigned PRGchip : 1;
			unsigned Mir_HV  : 1;
			unsigned PRG     : 6;
			unsigned         : 2;

			unsigned CHRlo   : 2;
			unsigned         : 6;
		};
		struct
		{
			unsigned addr    :16;
			unsigned data    : 8;
		};
	}	Addr;
	Addr.addr = Latch.Addr;
	Addr.data = Latch.Data;

	EMU->SetCHR_ROM8(0,(Addr.CHRhi << 2) | Addr.CHRlo);

	if (Addr.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(Addr.PRGchip << 6) | Addr.PRG);
		EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 6) | Addr.PRG);
	}
	else	EMU->SetPRG_ROM32(0x8,((Addr.PRGchip << 6) | Addr.PRG) >> 1);

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
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_BMC_Super700in1 =
{
	-1,
	"BMC-Super700in1",
	"BMC-Super700in1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AD,
	NULL,
	NULL
};