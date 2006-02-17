#include	"..\DLL\d_iNES.h"

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetPRG_ROM32(0x8,0);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0,0);
	else	EMU->SetCHR_RAM8(0,0);
}

static	u8 MapperNum = 0;
CTMapperInfo	MapperInfo_000 =
{
	&MapperNum,
	"NROM",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};