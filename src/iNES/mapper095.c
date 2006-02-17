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
}

static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (MMC3_GetCHRBank((Addr >> 10) & 0x7) & 0x20)
	{
		if (Mapper.LastMirror == 0)
		{
			EMU->Mirror_S1();
			Mapper.LastMirror = 1;
		}
	}
	else
	{
		if (Mapper.LastMirror == 1)
		{
			EMU->Mirror_S0();
			Mapper.LastMirror = 0;
		}
	}
	MMC3_PPUCycle(Addr,Scanline,Cycle,IsRendering);
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
	PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};