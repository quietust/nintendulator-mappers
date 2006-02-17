#include	"..\DLL\d_iNES.h"

static	struct
{
	int WhichGame;
	int WhichBank;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.WhichGame | Mapper.WhichBank);
	EMU->SetPRG_ROM16(0xC,Mapper.WhichGame | 3);
	EMU->SetCHR_RAM8(0,0);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichGame)
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichBank)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	Mapper.WhichGame = (What & 0x18) >> 1;
	Sync();
}

static	void	_MAPINT	WriteABCDEF (int Bank, int Where, int What)
{
	Mapper.WhichBank = What & 3;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xB,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xC,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xD,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xE,WriteABCDEF);
	EMU->SetCPUWriteHandler(0xF,WriteABCDEF);

	Mapper.WhichGame = 0;
	Mapper.WhichBank = 3;

	Sync();
}

static	u8 MapperNum = 232;
CTMapperInfo	MapperInfo_232 =
{
	&MapperNum,
	"Camerica 9096",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};