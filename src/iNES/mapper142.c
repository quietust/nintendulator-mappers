#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRGcontrol;
	u8 PRG[4];
	u16_n IRQcounter;
	u8 IRQenabled;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
	EMU->SetPRG_ROM8(0x6,Mapper.PRG[3]);
	EMU->SetPRG_ROM8(0xE,-1);
	EMU->SetCHR_RAM8(0,0);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGcontrol)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		if (Mapper.IRQcounter.s0 == 0xFFFF)
		{
			Mapper.IRQenabled = 0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter.s0++;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n0 = What & 0xF;
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n1 = What & 0xF;
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n2 = What & 0xF;
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n3 = What & 0xF;
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	Mapper.IRQenabled = What & 0xF;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	Mapper.PRGcontrol = What & 0xF;
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	switch (Mapper.PRGcontrol)
	{
	case 0x1:	Mapper.PRG[0] = What & 0xF;	break;
	case 0x2:	Mapper.PRG[1] = What & 0xF;	break;
	case 0x3:	Mapper.PRG[2] = What & 0xF;	break;
	case 0x4:	Mapper.PRG[3] = What & 0xF;	break;
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

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	for (x = 0; x < 4; x++)
		Mapper.PRG[x] = 0;
	Mapper.PRGcontrol = 0;

	Sync();
}

static	u8 MapperNum = 142;
CTMapperInfo	MapperInfo_142 =
{
	&MapperNum,
	"SMB2j Pirate (KS 202)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};