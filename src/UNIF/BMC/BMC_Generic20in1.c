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
		u16 addr;
	}	M;
	M.addr = Latch.Addr.s0;
	
	EMU->SetCHR_RAM8(0,0);
	if (M.PRGsize)
		EMU->SetPRG_ROM32(0x8,M.PRG);
	else
	{
		EMU->SetPRG_ROM16(0x8,M.PRG << 1);
		EMU->SetPRG_ROM16(0x8,M.PRG << 1);
	}
	switch (M.Mirror)
	{
	case 0:	EMU->Mirror_S0();		break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_H();		break;
	case 3:	EMU->Mirror_Custom(0,1,1,1);	break;
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

CTMapperInfo	MapperInfo_BMC_Generic20in1 =
{
	"BMC-Generic20in1",
	_T("Pirate multicart mapper"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};