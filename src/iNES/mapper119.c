#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	u8 x, y;
	MMC3_SyncMirror();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncWRAM();
	for (x = 0; x < 8; x++)
	{
		y = MMC3_GetCHRBank(x);
		if (y & 0x40)
			EMU->SetCHR_RAM1(x,y & 0x07);
		else	EMU->SetCHR_ROM1(x,y & 0x3F);
	}
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 119;
CTMapperInfo	MapperInfo_119 =
{
	&MapperNum,
	_T("TQROM (MMC3)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};