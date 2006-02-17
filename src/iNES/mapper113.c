#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG, CHR;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	u16 Loc = (Bank << 12) | Where;
	if (Loc < 0x4018)
	{
		Mapper.Write4(Bank,Where,What);
		return;
	}
	switch (Loc)
	{
	case 0x4120:	Mapper.PRG = (What & 0x38) >> 3;
			Mapper.CHR = (What & 0xC0) >> 3 | (What & 0x07);
			Sync();
			break;
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

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.PRG = 0;
	Mapper.CHR = 0;

	Sync();
}

static	u8 MapperNum = 113;
CTMapperInfo	MapperInfo_113 =
{
	&MapperNum,
	"Mapper 113 (HES)",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};