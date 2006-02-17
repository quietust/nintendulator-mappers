#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 4) & 0x3);
	EMU->SetCHR_ROM8(0,(Latch.Data >> 0) & 0x3);
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	EMU->SetCPUWriteHandler(0x6,Latch_Write);
	EMU->SetCPUWriteHandler(0x7,Latch_Write);
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 66;
CTMapperInfo	MapperInfo_066 =
{
	&MapperNum,
	_T("GNROM/compatible"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};