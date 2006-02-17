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
		struct
		{
			unsigned n       :16;
		};
	}	Addr;
	Addr.n = Latch.Addr;

	EMU->SetCHR_RAM8(0,0);

	if (Addr.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	if (!Addr.PRGsize) 
	{
		EMU->SetPRG_ROM16(0x8,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) | (Addr.PRG16));
		EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) | (Addr.PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8,(Addr.PRGchip << 4) | (Addr.PRGbank));

	if (Addr.CHRprot)
		;	/* Protect CHR */
	else
	{	/* Unprotect CHR */
		if (Addr.LastBank)
			EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) |  7);
		else	EMU->SetPRG_ROM16(0xC,(Addr.PRGchip << 5) | (Addr.PRGbank << 1) & ~7);
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

CTMapperInfo	MapperInfo_227 =
{
	227,
	NULL,
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