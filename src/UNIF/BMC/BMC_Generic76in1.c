#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	u8 Regs[2];
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
			unsigned byte0   : 8;
			unsigned byte1   : 8;
		};
	}	Latch;

	Latch.byte0 = Mapper.Regs[0];
	Latch.byte1 = Mapper.Regs[1];

	if (Latch.Mir_VH)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (Latch.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,((Latch.PRGhi) << 5) | (Latch.PRGbank));
		EMU->SetPRG_ROM16(0xC,((Latch.PRGhi) << 5) | (Latch.PRGbank));
	}
	else	EMU->SetPRG_ROM32(0x8,((Latch.PRGhi) << 4) | (Latch.PRGbank >> 1));
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.Regs[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	Mapper.Regs[Where & 1] = What;
	Sync();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Write);
	for (x = 0; x < 2; x++)	Mapper.Regs[x] = 0;
	Sync();
}

CTMapperInfo	MapperInfo_BMC_Generic76in1 =
{
	"BMC-Generic76in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};