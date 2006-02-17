#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write;
	u8 CHR;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank,int Where,int What)
{
	if (Where == 0x016)
	{
		Mapper.CHR = (What & 0x04) >> 2;
		Sync();
	}
	Mapper.Write(Bank,Where,What);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);

	Mapper.CHR = 0;

	Sync();
}

CTMapperInfo	MapperInfo_099 =
{
	99,
	NULL,
	"VS Unisystem",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};