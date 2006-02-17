#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Latch.Data >> 3) & 0x18);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM8 (0,Latch.Data & 0x7);
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

CTMapperInfo	MapperInfo_008 =
{
	8,
	NULL,
	"FFE F3xxx",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};