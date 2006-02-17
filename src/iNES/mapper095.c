#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0x1F,0);
	EMU->SetCHR_NT1(0x8,(MMC3_GetCHRBank(0) & 0x20) >> 5);
	EMU->SetCHR_NT1(0x9,(MMC3_GetCHRBank(1) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xA,(MMC3_GetCHRBank(2) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xB,(MMC3_GetCHRBank(3) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xC,(MMC3_GetCHRBank(4) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xD,(MMC3_GetCHRBank(5) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xE,(MMC3_GetCHRBank(6) & 0x20) >> 5);
	EMU->SetCHR_NT1(0xF,(MMC3_GetCHRBank(7) & 0x20) >> 5);
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

static	u8 MapperNum = 95;
CTMapperInfo	MapperInfo_095 =
{
	&MapperNum,
	"Dragon Buster (MMC3 variant)",
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};