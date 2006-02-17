#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned Mir_HV  : 1;
			unsigned PRGsizeL: 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 7;
			unsigned         : 1;
			unsigned PRGsizeH: 1;
			unsigned         : 4;
		};
		struct
		{
			unsigned addr    :16;
		};
	}	latch;
	u8 x;
	latch.addr = Latch.Addr;

	EMU->SetCHR_RAM8(0,0);
	
	if ((latch.PRGbank & 0x60) == 0x60)
		for (x = 0x8; x < 0x10; x++)
			EMU->SetPRG_OB4(x);
	switch ((latch.PRGsizeH << 1) | latch.PRGsizeL)
	{
	case 0:	EMU->SetPRG_ROM16(0x8,(latch.PRGbank << 1) | latch.PRG16);
		EMU->SetPRG_ROM16(0xC,(latch.PRGbank << 1) | latch.PRG16);
							break;
	case 1:	EMU->SetPRG_ROM32(0x8,latch.PRGbank);	break;

	case 2:	EMU->SetPRG_ROM8(0x8,((latch.PRGbank & 0x7F) << 2) | (latch.PRG16 << 1) | 0);
		EMU->SetPRG_ROM8(0xA,((latch.PRGbank & 0x7F) << 2) | (latch.PRG16 << 1) | 1);
		EMU->SetPRG_ROM8(0xC,((latch.PRGbank & 0x7F) << 2) | (latch.PRG16 << 1) | 0);
		EMU->SetPRG_ROM8(0xE,((latch.PRGbank & 0x1F) << 2) | (latch.PRG16 << 1) | 1);
							break;
	case 3:	EMU->SetPRG_ROM8(0x8,((latch.PRGbank & 0x7F) << 2) | 0);
		EMU->SetPRG_ROM8(0xA,((latch.PRGbank & 0x7F) << 2) | 1);
		EMU->SetPRG_ROM8(0xC,((latch.PRGbank & 0x7F) << 2) | 2);
		EMU->SetPRG_ROM8(0xE,((latch.PRGbank & 0x1F) << 2) | 3);
							break;
	}
	
	if (latch.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();

	if (latch.PRGbank == 0) 
	{
		EMU->GetPRG_Ptr4(0xF)[0x02A7] = (u8)0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x02A8] = (u8)0xF2;
		EMU->GetPRG_Ptr4(0xF)[0x028A] = (u8)0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x028B] = (u8)0xF2;
	}
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

static	u8 MapperNum = 63;
CTMapperInfo	MapperInfo_063 =
{
	&MapperNum,
	"Hello Kitty 255 in 1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};