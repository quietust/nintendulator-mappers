#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Addr & 0x3);
	EMU->SetCHR_ROM8(0,Latch.Addr & 0x7);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	Latch_Init(Sync,IsHardReset,FALSE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_BMC_NovelDiamond9999999in1 =
{
	-1,
	"BMC-NovelDiamond9999999in1",
	"BMC-NovelDiamond9999999in1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_AL,
	NULL,
	NULL
};