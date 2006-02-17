#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	WriteCDEF (int Bank, int Addr, int Val)
{
	Mapper.PRG = Val;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Val & 0x10)
		EMU->Mirror_S1();
	else	EMU->Mirror_S0();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x9,Write9);	/* Fire Hawk needs this */
	EMU->SetCPUWriteHandler(0xC,WriteCDEF);
	EMU->SetCPUWriteHandler(0xD,WriteCDEF);
	EMU->SetCPUWriteHandler(0xE,WriteCDEF);
	EMU->SetCPUWriteHandler(0xF,WriteCDEF);

	Mapper.PRG = 0;

	Sync();
}

static	u8 MapperNum = 71;
CTMapperInfo	MapperInfo_071 =
{
	&MapperNum,
	"Camerica BF9093/BF9097",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};