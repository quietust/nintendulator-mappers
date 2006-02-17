#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_VRC7.h"

static	struct
{
	u8 IRQenabled, IRQlatch, IRQcounter;
	u8 PRG[3], CHR[8], Misc;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	if (Mapper.Misc & 0x80)
		EMU->SetPRG_RAM8(0x6,0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
	if (ROM->INES_CHRSize)
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	else	for (x = 0; x < 8; x++)
			EMU->SetCHR_RAM1(x,Mapper.CHR[x] & 7);
	switch (Mapper.Misc & 0x3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Misc)
	x = VRC7sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

#define IRQ_CYCLES 341
static int IRQcycles = IRQ_CYCLES;
static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled & 2) && ((Mapper.IRQenabled & 4) || ((IRQcycles -= 3) < 0)))
	{
		if (!(Mapper.IRQenabled & 4))
			IRQcycles += IRQ_CYCLES;
		if (Mapper.IRQcounter == 0xFF)
		{
			Mapper.IRQcounter = Mapper.IRQlatch;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.PRG[1] = What;
	else	Mapper.PRG[0] = What;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	if (Where & 0x18)
		VRC7sound_Write((Bank << 12) | Where,What);
	else
	{
		Mapper.PRG[2] = What;
		Sync();
	}
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.CHR[1] = What;
	else	Mapper.CHR[0] = What;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.CHR[3] = What;
	else	Mapper.CHR[2] = What;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.CHR[5] = What;
	else	Mapper.CHR[4] = What;
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.CHR[7] = What;
	else	Mapper.CHR[6] = What;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	if (Where & 0x18)
		Mapper.IRQlatch = What;
	else
	{
		Mapper.Misc = What;
		Sync();
	}
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	if (Where & 0x18)
	{
		if (Mapper.IRQenabled & 1)
			Mapper.IRQenabled |= 0x2;
	}
	else
	{
		Mapper.IRQenabled = What & 0x7;
		if (Mapper.IRQenabled & 0x2)
		{
			Mapper.IRQcounter = Mapper.IRQlatch;
			IRQcycles = IRQ_CYCLES;
		}
	}
	EMU->SetIRQ(1);
}


static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	VRC7sound_Destroy();
}

static	int	_MAPINT	MapperSnd (int Cycles)
{
	return VRC7sound_Get(Cycles);
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
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.IRQenabled = Mapper.IRQlatch = Mapper.IRQcounter = 0;
	Mapper.PRG[0] = 0x00;	Mapper.PRG[0] = 0x01;	Mapper.PRG[2] = 0xFE;
	for (x = 0; x < 8; x++)	Mapper.CHR[x] = x;

	VRC7sound_Init();
	Sync();
}

static	u8 MapperNum = 85;
CTMapperInfo	MapperInfo_085 =
{
	&MapperNum,
	"Konami VRC7",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};