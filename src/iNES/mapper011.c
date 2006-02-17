#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 0) & 0xF);
	EMU->SetCHR_RAM8 (0,0);	// there exists a mapper11 game with CHR RAM
	EMU->SetCHR_ROM8 (0,(Latch.Data >> 4) & 0xF);
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

CTMapperInfo	MapperInfo_011 =
{
	11,
	NULL,
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
