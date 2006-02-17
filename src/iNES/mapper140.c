#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Latch;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch >> 4) & 0x3);
	EMU->SetCHR_ROM8(0,(Mapper.Latch >> 0) & 0xF);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Latch = Val;
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	if (ResetType == RESET_HARD)
		Mapper.Latch = 0;

	Sync();
}

static	u8 MapperNum = 140;
CTMapperInfo	MapperInfo_140 =
{
	&MapperNum,
	"Mapper 140 (Bio Senshi Dan)",
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};