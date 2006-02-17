#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Game;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Mapper.Game);
	EMU->SetPRG_ROM16(0x8,Mapper.Game);
	EMU->SetPRG_ROM16(0xC,Mapper.Game);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Game)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	if (IsHardReset)
		Mapper.Game = 0;
	else	Mapper.Game++;

	Sync();
}

static	u8 MapperNum = 60;
CTMapperInfo	MapperInfo_060 =
{
	&MapperNum,
	"Reset-triggered 4-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};