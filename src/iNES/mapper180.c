#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,Latch.Data & 0x7);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0,0);
	else	EMU->SetCHR_RAM8(0,0);
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

static	u8 MapperNum = 180;
CTMapperInfo	MapperInfo_180 =
{
	&MapperNum,
	"Mapper 180",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};