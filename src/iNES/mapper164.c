#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 Latch;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Mapper.Latch & 0x7);
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch & 0x8) >> 3);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	if (Where & 0x100)
	{
		Mapper.Latch = What;
		Sync();
	}
	if (Bank == 4) Mapper.Write4(Bank,Where,What);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);
	EMU->SetCPUWriteHandler(0x5,Write);

	Mapper.Latch = 0;

	Sync();
}

static	u8 MapperNum = 79;
CTMapperInfo	MapperInfo_079 =
{
	&MapperNum,
	"NINA-03/NINA-06",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};