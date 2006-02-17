#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Reg & 0x30) >> 4);
	EMU->SetCHR_ROM8(0,(Mapper.Reg & 0x03) | ((Mapper.Reg & 0x40) >> 4));
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	WriteBank (int Bank, int Where, int What)
{
	Mapper.Reg = What;
	Sync();
}

static	void	_MAPINT	WriteSpeech (int Bank, int Where, int What)
{
	/* Most likely never to be implemented */
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,WriteBank);
	EMU->SetCPUWriteHandler(0x7,WriteSpeech);

	Mapper.Reg = 0;

	Sync();
}

static	u8 MapperNum = 86;
CTMapperInfo	MapperInfo_086 =
{
	&MapperNum,
	"Mapper 86 (Jaleco)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};