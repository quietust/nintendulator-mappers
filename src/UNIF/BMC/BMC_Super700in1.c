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
	}	M;
	M.addr = Latch.Addr.s0;
	M.data = Latch.Data;

	EMU->SetCHR_ROM8(0,(M.CHRhi << 2) | M.CHRlo);

	if (M.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(M.PRGchip << 6) | M.PRG);
		EMU->SetPRG_ROM16(0xC,(M.PRGchip << 6) | M.PRG);
	}
	else	EMU->SetPRG_ROM32(0x8,((M.PRGchip << 6) | M.PRG) >> 1);

	if (M.Mir_HV)
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
	"BMC-Super700in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AD,
	NULL,
	NULL
};