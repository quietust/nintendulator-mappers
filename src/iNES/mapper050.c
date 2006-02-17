#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,0xF);
	EMU->SetPRG_ROM8(0x8,0x8);
	EMU->SetPRG_ROM8(0xA,0x9);
	EMU->SetPRG_ROM8(0xC,((Mapper.PRG & 0x1) << 2) | ((Mapper.PRG & 0x6) >> 1) | (Mapper.PRG & 0x8));
	EMU->SetPRG_ROM8(0xE,0xB);
	EMU->SetCHR_RAM8(0,0);
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
		if (Mapper.IRQcounter.s0 >= 4096)
			EMU->SetIRQ(0);
	}
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Where,What);
	if ((Where & 0x060) != 0x020)
		return;
	if (Where & 0x100)
	{
		if (What & 1)
			Mapper.IRQenabled = 1;
		else
		{
			Mapper.IRQcounter.s0 = 0;
			Mapper.IRQenabled = 0;
			EMU->SetIRQ(1);
		}
	}
	else
	{
		Mapper.PRG = What;
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

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
		EMU->SetCPUWriteHandler(x,Write);
	Mapper.PRG = 0;
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;

	Sync();
}

CTMapperInfo	MapperInfo_050 =
{
	50,
	NULL,
	"SMB2j rev. A",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
