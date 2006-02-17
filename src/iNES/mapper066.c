#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetCHR_ROM8 (  0,(Latch.Data >> 0) & 0x3);
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x3);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,Latch_Write);
	EMU->SetCPUWriteHandler(0x7,Latch_Write);
	Latch_Init(ResetType,Sync,FALSE);
}

static	u8 MapperNum = 66;
CTMapperInfo	MapperInfo_066 =
{
	&MapperNum,
	"GNROM/compatible",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};