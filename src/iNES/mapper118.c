#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0x7F,0);
	EMU->SetCHR_NT1(0x8,(MMC3_GetCHRBank(0) & 0x80) >> 7);
	EMU->SetCHR_NT1(0x9,(MMC3_GetCHRBank(1) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xA,(MMC3_GetCHRBank(2) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xB,(MMC3_GetCHRBank(3) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xC,(MMC3_GetCHRBank(4) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xD,(MMC3_GetCHRBank(5) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xE,(MMC3_GetCHRBank(6) & 0x80) >> 7);
	EMU->SetCHR_NT1(0xF,(MMC3_GetCHRBank(7) & 0x80) >> 7);
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 118;
CTMapperInfo	MapperInfo_118 =
{
	&MapperNum,
	_T("TKSROM/TLSROM (MMC3)"),
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