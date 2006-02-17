#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Byte8000, Byte8001;
}	Mapper;

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_VH  : 1;
			unsigned PRGhi   : 2;
			unsigned         : 7;
		};
		struct
		{
			u8 b[2];
		};
	}	Val;

	Val.b0 = Mapper.Byte8000;
	Val.b1 = Mapper.Byte8001;

	if (Val.Mir_VH)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (Val.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,((Val.PRGhi) << 5) | (Val.PRGbank));
		EMU->SetPRG_ROM16(0xC,((Val.PRGhi) << 5) | (Val.PRGbank));
	}
	else	EMU->SetPRG_ROM32(0x8,((Val.PRGhi) << 4) | (Val.PRGbank >> 1));
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte8000)
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte8001)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	switch (Where & 1)
	{
	case 0:	Mapper.Byte8000 = What;		break;
	case 1:	Mapper.Byte8001 = What;		break;
	}
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

	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Byte8000 = Mapper.Byte8001 = 0;

	Sync();
}

CTMapperInfo	MapperInfo_226 =
{
	226,
	NULL,
	"76-in-1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};