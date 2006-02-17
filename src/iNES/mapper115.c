#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncMirror();
	if (Mapper.Reg & 1)
		MMC3_SyncPRG(0xF,(Mapper.Reg & 0xC0) >> 2);
	else	EMU->SetPRG_ROM32(0x8,(Mapper.Reg & 0x30) >> 4);
	MMC3_SyncCHR_ROM(0x7F,(Mapper.Reg & 0xC0) << 1);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	Mapper.Reg = What;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	iNES_InitROM();

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(0x6,Write);
	Mapper.Reg = 0;
	MMC3_Init(Sync);
}

static	u8 MapperNum = 49;
CTMapperInfo	MapperInfo_049 =
{
	&MapperNum,
	"1993 Super HiK 4-in-1 (MMC3)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};