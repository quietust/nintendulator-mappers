#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

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
	iNES_UnloadROM();
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Latch_Init(Sync,IsHardReset,FALSE);
}

static	u8 MapperNum = 62;
CTMapperInfo	MapperInfo_062 =
{
	&MapperNum,
	"700-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AD,
	NULL,
	NULL
};