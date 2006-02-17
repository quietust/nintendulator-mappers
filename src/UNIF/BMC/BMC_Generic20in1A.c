#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

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

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

CTMapperInfo	MapperInfo_BMC_Generic20in1A =
{
	"BMC-Generic20in1A",
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