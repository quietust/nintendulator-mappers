#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data >> 0) & 0x3);
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM8(0,(Latch.Data >> 4) & 0xF);
	else	EMU->SetCHR_RAM8(0,0);
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	Latch_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 11;
CTMapperInfo	MapperInfo_011 =
{
	&MapperNum,
	_T("Color Dreams"),
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
