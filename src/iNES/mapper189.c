#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 Latch;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.Latch);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0x3F,0);
	MMC3_SyncMirror();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write4 (int Bank, int Where, int What)
{
	if (Where == 0x120)
	{
		Mapper.Latch = What >> 4;
		Sync();
	}
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write4);

	EMU->SetPRG_RAM8(0x6,0);
	Mapper.Latch = 0;

	MMC3_Init(Sync);
}

static	u8 MapperNum = 189;
CTMapperInfo	MapperInfo_189 =
{
	&MapperNum,
	"Mapper 189 (Thunder Warrior)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};