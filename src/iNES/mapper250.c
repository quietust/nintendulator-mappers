#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	MMC3_SyncMirror();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	MMC3_CPUWrite89(Bank,Where >> 10,Where & 0xFF);
}
static	void	_MAPINT	WriteAB (int Bank, int Where, int What)
{
	MMC3_CPUWriteAB(Bank,Where >> 10,Where & 0xFF);
}
static	void	_MAPINT	WriteCD (int Bank, int Where, int What)
{
	MMC3_CPUWriteCD(Bank,Where >> 10,Where & 0xFF);
}
static	void	_MAPINT	WriteEF (int Bank, int Where, int What)
{
	MMC3_CPUWriteEF(Bank,Where >> 10,Where & 0xFF);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetPRG_RAM8(0x6,0);

	MMC3_Init(Sync);

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
}

CTMapperInfo	MapperInfo_250 =
{
	250,
	NULL,
	"Time Diver Avenger (MMC3 Variant)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};