#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"

static	void	Sync (void)
{
	MMC1_SyncMirror();
	if (ROM->INES_PRGSize == 32)
		MMC1_SyncPRG(0xF,MMC1_GetCHRBankLo() & 0x10);
	else	MMC1_SyncPRG(0xF,0);
	if (ROM->INES_CHRSize)
		MMC1_SyncCHR_ROM(0x1F,0);
	else	MMC1_SyncCHR_RAM(0x01,0);
	MMC1_SyncWRAM();
}

static	void	_MAPINT	Load (void)
{
	MMC1_Load(Sync);
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC1_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC1_Unload();
}

static	u8 MapperNum = 1;
CTMapperInfo	MapperInfo_001 =
{
	&MapperNum,
	"MMC1",
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	MMC1_SaveLoad,
	NULL,
	NULL
};