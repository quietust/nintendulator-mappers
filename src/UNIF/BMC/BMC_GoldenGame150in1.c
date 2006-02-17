#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

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
		struct
		{
			unsigned n       : 16;
		};
	}	Addr;
	u8 openbus = 0;
	Addr.n = Latch.Addr;
	if (ROM->UNIF_NumPRG == 1)	/* 1MB, 100-in-1 */
	{
		switch (Addr.PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	openbus = 1;	break;
		case 3:	openbus = 1;	break;
		}
	}
	else if (ROM->UNIF_NumPRG == 2)	/* 2MB, 150-in-1 */
	{
		switch (Addr.PRGchip)
		{
		case 0:			break;
		case 1:	openbus = 1;	break;
		case 2:	Addr.PRGchip = 1;
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
		if (Addr.PRGsize)
		{
			EMU->SetPRG_ROM16(0x8,(Addr.PRGchip << 6) | (Addr.PRGbank << 1) | (Addr.PRG16));
			EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 6) | (Addr.PRGbank << 1) | (Addr.PRG16));
		}
		else	EMU->SetPRG_ROM32(0x8,(Addr.PRGchip << 5) | Addr.PRGbank);
	}
	EMU->SetCHR_RAM8(0,0);
	if (Addr.Mir_S0)
		EMU->Mirror_S0();
	else if (Addr.Mir_HV)
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

CTMapperInfo	MapperInfo_BMC_GoldenGame150in1 =
{
	"BMC-GoldenGame150in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};