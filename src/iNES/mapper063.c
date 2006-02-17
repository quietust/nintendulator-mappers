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
		u16 addr;
	}	M;
	u8 x;
	M.addr = Latch.Addr.s0;

	EMU->SetCHR_RAM8(0,0);
	
	if ((M.PRGbank & 0x60) == 0x60)
		for (x = 0x8; x < 0x10; x++)
			EMU->SetPRG_OB4(x);
	switch ((M.PRGsizeH << 1) | M.PRGsizeL)
	{
	case 0:	EMU->SetPRG_ROM16(0x8,(M.PRGbank << 1) | M.PRG16);
		EMU->SetPRG_ROM16(0xC,(M.PRGbank << 1) | M.PRG16);
							break;
	case 1:	EMU->SetPRG_ROM32(0x8,M.PRGbank);	break;

	case 2:	EMU->SetPRG_ROM8(0x8,((M.PRGbank & 0x7F) << 2) | (M.PRG16 << 1) | 0);
		EMU->SetPRG_ROM8(0xA,((M.PRGbank & 0x7F) << 2) | (M.PRG16 << 1) | 1);
		EMU->SetPRG_ROM8(0xC,((M.PRGbank & 0x7F) << 2) | (M.PRG16 << 1) | 0);
		EMU->SetPRG_ROM8(0xE,((M.PRGbank & 0x1F) << 2) | (M.PRG16 << 1) | 1);
							break;
	case 3:	EMU->SetPRG_ROM8(0x8,((M.PRGbank & 0x7F) << 2) | 0);
		EMU->SetPRG_ROM8(0xA,((M.PRGbank & 0x7F) << 2) | 1);
		EMU->SetPRG_ROM8(0xC,((M.PRGbank & 0x7F) << 2) | 2);
		EMU->SetPRG_ROM8(0xE,((M.PRGbank & 0x1F) << 2) | 3);
							break;
	}
	
	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();

	if (M.PRGbank == 0) 
	{	// hack
		EMU->GetPRG_Ptr4(0xF)[0x02A7] = 0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x02A8] = 0xF2;
		EMU->GetPRG_Ptr4(0xF)[0x028A] = 0x6F;
		EMU->GetPRG_Ptr4(0xF)[0x028B] = 0xF2;
	}
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