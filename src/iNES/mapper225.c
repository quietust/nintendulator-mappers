#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

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
		u16 addr;
	}	M;
	M.addr = Latch.Addr.s0;

	EMU->SetCHR_ROM8(0,M.CHRbank);
	if (M.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(M.PRGbank << 1) | (M.PRG16));
		EMU->SetPRG_ROM16(0xC,(M.PRGbank << 1) | (M.PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8,M.PRGbank);
	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
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

static	u8 MapperNum = 225;
CTMapperInfo	MapperInfo_225 =
{
	&MapperNum,
	"72-in-1",
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};