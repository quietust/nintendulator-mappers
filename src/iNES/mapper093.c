#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data & 0xF0) >> 4);
	EMU->SetPRG_ROM16(0xC,-1);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0,0);
	else	EMU->SetCHR_RAM8(0,0);
	if (Latch.Data & 0x01)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	Latch_Init(ResetType,Sync,FALSE);
}

static	u8 MapperNum = 93;
CTMapperInfo	MapperInfo_093 =
{
	&MapperNum,
	"Mapper 93",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};