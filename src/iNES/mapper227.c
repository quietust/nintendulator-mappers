#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRGsize : 1;
			unsigned Mir_HV  : 1;
			unsigned PRG16   : 1;
			unsigned PRGbank : 4;
			unsigned CHRprot : 1;
			unsigned PRGchip : 1;
			unsigned LastBank: 1;
			unsigned         : 9;
		};
		u16 addr;
	}	M;
	M.addr = Latch.Addr.s0;

	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (M.PRGsize) 
		EMU->SetPRG_ROM32(0x8,(M.PRGchip << 4) | (M.PRGbank));
	else
	{
		EMU->SetPRG_ROM16(0x8,(M.PRGchip << 5) | (M.PRGbank << 1) | (M.PRG16));
		EMU->SetPRG_ROM16(0xC,(M.PRGchip << 5) | (M.PRGbank << 1) | (M.PRG16));
	}

	if (M.CHRprot)
	{
		EMU->SetCHR_RAM8(0,0);
		;	/* Protect CHR */
	}
	else
	{	EMU->SetCHR_RAM8(0,0);
		if (M.LastBank)
			EMU->SetPRG_ROM16(0xC,(M.PRGchip << 5) | (M.PRGbank << 1) |  7);
		else	EMU->SetPRG_ROM16(0xC,(M.PRGchip << 5) | (M.PRGbank << 1) & ~7);
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

static	u8 MapperNum = 227;
CTMapperInfo	MapperInfo_227 =
{
	&MapperNum,
	"1200-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};