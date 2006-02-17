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
		u16 addr;
	}	M;
	M.addr = Latch.Addr.s0;
	EMU->SetCHR_RAM8(0,0);
	if (M.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(M.PRG << 1) | M.PRG16);
		EMU->SetPRG_ROM16(0xC,(M.PRG << 1) | M.PRG16);
	}
	else	EMU->SetPRG_ROM32(0x8,M.PRG);
	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();
	Latch_Init(ResetType,Sync,FALSE);
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