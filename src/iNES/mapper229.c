#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	if (Latch.Addr & 0x1E)
	{
		EMU->SetPRG_ROM16(0x8,Latch.Addr & 0x1F);
		EMU->SetPRG_ROM16(0xC,Latch.Addr & 0x1F);
	}
	else	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,Latch.Addr & 0x1F);
	if (Latch.Addr & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
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

static	u8 MapperNum = 229;
CTMapperInfo	MapperInfo_229 =
{
	&MapperNum,
	"31-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AL,
	NULL,
	NULL
};