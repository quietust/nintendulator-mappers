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
		if ((y & 0xFE) == ((ROM->INES_PRGSize >> 2) & 0x08))
			EMU->SetCHR_RAM1(x,y & 0x01);
		else	EMU->SetCHR_ROM1(x,y & 0xFF);
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

static	u8 MapperNum = 74;
CTMapperInfo	MapperInfo_074 =
{
	&MapperNum,
	_T("Mapper 74 (Taiwan MMC3)"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};