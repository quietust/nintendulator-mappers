#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	if (Mapper.Reg0 & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetPRG_ROM32(0x8,Mapper.Reg0 & 0x7);
	EMU->SetCHR_ROM8(0,Mapper.Reg1 | ((Mapper.Reg0 >> 1) & 0xC));
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	WriteLo (int Bank, int Where, int What)
{
	if (Where & 0x800)
		return;
	Mapper.Reg0 = Where & 0x3F;
	Sync();
}

static	void	_MAPINT	WriteHi (int Bank, int Where, int What)
{
	if (Mapper.Reg0 & 0x4)
	{
		Mapper.Reg1 = What & 0x03;
		Sync();
	}
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,WriteLo);
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,WriteHi);

	Mapper.Reg0 = Mapper.Reg1 = 0;
	Sync();
}

CTMapperInfo	MapperInfo_041 =
{
	41,
	NULL,
	"Caltron 6-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};