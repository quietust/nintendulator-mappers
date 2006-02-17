#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 LastMirror;
}	Mapper;

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

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}


static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.LastMirror = 0;
	EMU->Mirror_S0();

	MMC3_Init(Sync);
}

static	u8 MapperNum = 95;
CTMapperInfo	MapperInfo_095 =
{
	&MapperNum,
	"Dragon Buster (MMC3 variant)",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};