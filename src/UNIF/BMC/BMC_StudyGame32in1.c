#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x1F);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	UNIF_SetMirroring(NULL);
	Latch_Init(ResetType,Sync,FALSE);
}

CTMapperInfo	MapperInfo_BMC_StudyGame32in1 =
{
	"BMC-StudyGame32in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};