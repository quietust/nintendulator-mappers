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
		u16 addr;
	}	M;
	u8 openbus = 0;
	M.addr = Latch.Addr.s0;

	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	
	switch (M.PRGchip)
	{
	case 2:	openbus = 1;	break;
	case 3:	M.PRGchip = 2;	break;
	}
	if (openbus)
	{
		for (openbus = 0x8; openbus <= 0xF; openbus++)
			EMU->SetPRG_OB4(openbus);
	}
	else
	{
		if (M.PRGsize)
		{
			EMU->SetPRG_ROM16(0x8,(M.PRGchip << 5) | (M.PRGbank << 1) | (M.PRG16));
			EMU->SetPRG_ROM16(0xC,(M.PRGchip << 5) | (M.PRGbank << 1) | (M.PRG16));
		}
		else EMU->SetPRG_ROM32(0x8,(M.PRGchip << 4) | (M.PRGbank));
	}
	EMU->SetCHR_ROM8(0,(M.CHRbank << 2) | (Latch.Data & 0x3));
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

static	u8 MapperNum = 228;
CTMapperInfo	MapperInfo_228 =
{
	&MapperNum,
	_T("Action 52"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_AD,
	NULL,
	NULL
};