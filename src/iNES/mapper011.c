#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 0) & 0x3);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0,(Latch.Data >> 4) & 0xF);
	else	EMU->SetCHR_RAM8(0,0);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();
	Latch_Init(Sync,IsHardReset,TRUE);
}

static	u8 MapperNum = 11;
CTMapperInfo	MapperInfo_011 =
{
	&MapperNum,
	"Color Dreams",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
