#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Bank, Mode;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mode & 1)
		EMU->SetPRG_ROM8(0x6,Mapper.Bank | 0x23);
	else	EMU->SetPRG_ROM8(0x6,Mapper.Bank | 0x2F);
	if (Mapper.Mode == 2)
		EMU->SetPRG_ROM16(0x8,(Mapper.Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0x8,Mapper.Bank >> 1);
	if (Mapper.Mode & 1)
		EMU->SetPRG_ROM16(0xC,(Mapper.Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0xC,(Mapper.Bank >> 1) | 7);
	if (Mapper.Mode == 3)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Bank)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write67 (int Bank, int Where, int What)
{
	Mapper.Mode = ((What & 0x10) >> 3) | ((What & 0x02) >> 1);
	Sync();
}

static	void	_MAPINT	Write89ABCDEF (int Bank, int Where, int What)
{
	Mapper.Bank = (What & 0xF) << 2;
	if (Bank & 0x4)
		Mapper.Mode = (Mapper.Mode & 0x1) | ((What & 0x10) >> 3);
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;

	iNES_InitROM();

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write67);
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write89ABCDEF);

	Mapper.Bank = 0;
	Mapper.Mode = 1;
	Sync();
}

CTMapperInfo	MapperInfo_051 =
{
	51,
	NULL,
	"11 in 1 Ball Games",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
