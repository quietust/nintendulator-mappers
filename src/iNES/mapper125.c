#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,Mapper.PRG);
	EMU->SetPRG_ROM8(0x8,0xC);
	EMU->SetPRG_ROM8(0xA,0xD);
	EMU->SetPRG_RAM8(0xC,0);
	EMU->SetPRG_ROM8(0xE,0xF);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}


static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.PRG = Val & 0xF;
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
		Mapper.PRG = 0;

	iNES_SetMirroring();
	Sync();
}

static	u8 MapperNum = 125;
CTMapperInfo	MapperInfo_125 =
{
	&MapperNum,
	_T("Monty on the Run (FDS hack)"),
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
