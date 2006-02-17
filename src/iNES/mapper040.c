#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,6);
	EMU->SetPRG_ROM8(0x8,4);
	EMU->SetPRG_ROM8(0xA,5);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG);
	EMU->SetPRG_ROM8(0xE,7);
	EMU->SetCHR_ROM8(0,0);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		Mapper.IRQcounter.s0++;
		if (Mapper.IRQcounter.s0 & 4096)
			EMU->SetIRQ(0);
	}
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteAB (int Bank, int Where, int What)
{
	Mapper.IRQenabled = 1;
}

static	void	_MAPINT	WriteEF (int Bank, int Where, int What)
{
	Mapper.PRG = What & 7;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);

	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;
	Mapper.PRG = 0;

	Sync();
}

CTMapperInfo	MapperInfo_040 =
{
	40,
	NULL,
	"SMB2j Pirate",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};