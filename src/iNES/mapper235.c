#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRGbank : 5;
			unsigned         : 3;
			unsigned PRGchip : 2;
			unsigned Mir_S0  : 1;	/* S0 if 1, H/V if 0 */
			unsigned PRGsize : 1;
			unsigned PRG16   : 1;
			unsigned Mir_HV  : 1;	/* H if 1, V if 0 */
			unsigned         : 2;
		};
		u16 addr;
	}	M;
	u8 openbus = 0;
	M.addr = Latch.Addr.s0;
	if (ROM->INES_PRGSize == 64)	/* 1MB, 100-in-1 */
	{
		switch (M.PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	openbus = 1;	break;
		case 3:	openbus = 1;	break;
		}
	}
	else if (ROM->INES_PRGSize == 128)	/* 2MB, 150-in-1 */
	{
		switch (M.PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	M.PRGchip = 1;
					break;
		case 3:	openbus = 1;	break;
		}
	}					/* else 260-in-1 */
	if (openbus)
	{
		for (openbus = 0x8; openbus < 0x10; openbus++)
			EMU->SetPRG_OB4(openbus);
	}
	else
	{
		if (M.PRGsize)
		{
			EMU->SetPRG_ROM16(0x8,(M.PRGchip << 6) | (M.PRGbank << 1) | (M.PRG16));
			EMU->SetPRG_ROM16(0xC,(M.PRGchip << 6) | (M.PRGbank << 1) | (M.PRG16));
		}
		else	EMU->SetPRG_ROM32(0x8,(M.PRGchip << 5) | M.PRGbank);
	}
	EMU->SetCHR_RAM8(0,0);
	if (M.Mir_S0)
		EMU->Mirror_S0();
	else if (M.Mir_HV)
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

static	u8 MapperNum = 235;
CTMapperInfo	MapperInfo_235 =
{
	&MapperNum,
	_T("Golden Game 150-in-1"),
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