#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[3], CHR[2];
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	EMU->SetCHR_ROM4(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM4(4,Mapper.CHR[1]);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	Mapper.PRG[0] = What;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	Mapper.PRG[1] = What;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	Mapper.PRG[2] = What;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	Mapper.CHR[0] = What;
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	Mapper.CHR[1] = What;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.PRG[0] = 0;
	Mapper.PRG[1] = 1;
	Mapper.PRG[2] = -2;
	Mapper.CHR[0] = 0;
	Mapper.CHR[1] = 1;

	Sync();
}

static	u8 MapperNum = 151;
CTMapperInfo	MapperInfo_151 =
{
	&MapperNum,
	"Extended VS Unisystem",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};