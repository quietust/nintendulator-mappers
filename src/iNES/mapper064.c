#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled, IRQcounter, IRQlatch, IRQmode, IRQreload;
	u16 IRQaddr;
	u8 Cmd;
	u8 PRG[3];
	u8 CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x, SwCHR = (Mapper.Cmd & 0x80) >> 5;
	if (Mapper.Mirror & 0x1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();

	if (Mapper.Cmd & 0x40)
	{
		EMU->SetPRG_ROM8(0xA,Mapper.PRG[0]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRG[1]);
		EMU->SetPRG_ROM8(0x8,Mapper.PRG[2]);
	}
	else
	{
		EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
		EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	}
	EMU->SetPRG_ROM8(0xE,-1);

	for (x = 4; x < 8; x++)
		EMU->SetCHR_ROM1(SwCHR ^ x,Mapper.CHR[x]);
	for (x = 0; x < 4; x++)
		EMU->SetCHR_ROM2(SwCHR ^ x,Mapper.CHR[x] >> 1);
	if (Mapper.Cmd & 0x20)
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(SwCHR ^ x,Mapper.CHR[x]);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	HBlank (void)
{
	if (!Mapper.IRQcounter || Mapper.IRQreload)
	{
		Mapper.IRQcounter = Mapper.IRQlatch + 1;
		if (Mapper.IRQreload)
			Mapper.IRQreload--;
	}
	else if (Mapper.IRQcounter)
		Mapper.IRQcounter--;
	if (!Mapper.IRQcounter)
	{
		if (Mapper.IRQenabled)
			EMU->SetIRQ(0);
		Mapper.IRQreload = 2;
	}
}

static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if (Addr & 0x2000)
		return;
	if (!(Mapper.IRQmode) && !(Mapper.IRQaddr & 0x1000) && (Addr & 0x1000))
		HBlank();
	Mapper.IRQaddr = Addr;
}

static int cycles = 4;
static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQmode) && (!--cycles))
	{
		cycles = 4;
		HBlank();
	}
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	if (Where & 1)
		switch (Mapper.Cmd & 0xF)
		{
		case 0:	Mapper.CHR[0] = What;	break;
		case 1:	Mapper.CHR[2] = What;	break;
		case 2:	Mapper.CHR[4] = What;	break;
		case 3:	Mapper.CHR[5] = What;	break;
		case 4:	Mapper.CHR[6] = What;	break;
		case 5:	Mapper.CHR[7] = What;	break;
		case 6:	Mapper.PRG[0] = What;	break;
		case 7:	Mapper.PRG[1] = What;	break;

		case 8:	Mapper.CHR[1] = What;	break;
		case 9:	Mapper.CHR[3] = What;	break;
		case 15:Mapper.PRG[2] = What;	break;
		}
	else	Mapper.Cmd = What;
	Sync();
}

static	void	_MAPINT	WriteAB (int Bank, int Where, int What)
{
	if (Where & 1)
		;
	else	Mapper.Mirror = What;
	Sync();
}

static	void	_MAPINT	WriteCD (int Bank, int Where, int What)
{
	if (Where & 1)
	{
		Mapper.IRQmode = What & 1;
		Mapper.IRQreload = 1;
	}
	else	Mapper.IRQlatch = What;
}

static	void	_MAPINT	WriteEF (int Bank, int Where, int What)
{
	Mapper.IRQenabled = (Where & 1);
	if (!Mapper.IRQenabled)
		EMU->SetIRQ(1);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;

	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);

	for (x = 0; x < 3; x++)	Mapper.PRG[x] = 0xFF;
	for (x = 0; x < 8; x++)	Mapper.CHR[x] = x;
	Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch = Mapper.IRQmode = Mapper.IRQreload = 0;
	Mapper.IRQaddr = 0;
	Mapper.Cmd = 0;
	Mapper.Mirror = 0;

	Sync();
}

static	u8 MapperNum = 64;
CTMapperInfo	MapperInfo_064 =
{
	&MapperNum,
	"Tengen RAMBO-1",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};
