#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
	else	MMC3_SyncMirror();
	MMC3_SyncPRG(0x3F,0);

	if (ROM->INES_CHRSize)
		MMC3_SyncCHR_ROM(0xFF,0);
	else	MMC3_SyncCHR_RAM(0x07,0);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetPRG_RAM8(0x6,0);	/* Map WRAM no matter what */

	MMC3_Init(Sync);
}

CTMapperInfo	MapperInfo_004 =
{
	4,
	NULL,
	"MMC3/MMC6",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};
