#include	"..\DLL\d_iNES.h"

static	struct
{
	u8_n PRG[3], CHR[8];
	u16 IRQcounter;
	u16_n IRQlatch;
	u8 IRQcontrol;
	u8 Mirror;
	u8 DisableSRAM;
	FCPURead ReadSRAM;
	FCPUWrite WriteSRAM;
}	Mapper;

static	u16 IRQmask;
static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x].b0);

	EMU->SetPRG_ROM8(0xE,0xFF);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x].b0);
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_H();	break;
	case 1:	EMU->Mirror_V();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (Mapper.IRQcontrol & 0x8)
		IRQmask = 0xF;
	else if (Mapper.IRQcontrol & 0x4)
		IRQmask = 0xFF;
	else if (Mapper.IRQcontrol & 0x2)
		IRQmask = 0xFFF;
	else	IRQmask = 0xFFFF;
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i].b0)
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i].b0)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQlatch.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcontrol)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	SAVELOAD_BYTE(mode,x,data,Mapper.DisableSRAM)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQcontrol & 1)
	{
		if (!(Mapper.IRQcounter & IRQmask))
			EMU->SetIRQ(0);
		Mapper.IRQcounter--;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.PRG[0].n0 = What & 0xF;	break;
	case 1:	Mapper.PRG[0].n1 = What & 0xF;	break;
	case 2:	Mapper.PRG[1].n0 = What & 0xF;	break;
	case 3:	Mapper.PRG[1].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.PRG[2].n0 = What & 0xF;	break;
	case 1:	Mapper.PRG[2].n1 = What & 0xF;	break;
	case 2:	Mapper.DisableSRAM = What & 1;	break;
	case 3:					break;
	}
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.CHR[0].n0 = What & 0xF;	break;
	case 1:	Mapper.CHR[0].n1 = What & 0xF;	break;
	case 2:	Mapper.CHR[1].n0 = What & 0xF;	break;
	case 3:	Mapper.CHR[1].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.CHR[2].n0 = What & 0xF;	break;
	case 1:	Mapper.CHR[2].n1 = What & 0xF;	break;
	case 2:	Mapper.CHR[3].n0 = What & 0xF;	break;
	case 3:	Mapper.CHR[3].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.CHR[4].n0 = What & 0xF;	break;
	case 1:	Mapper.CHR[4].n1 = What & 0xF;	break;
	case 2:	Mapper.CHR[5].n0 = What & 0xF;	break;
	case 3:	Mapper.CHR[5].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.CHR[6].n0 = What & 0xF;	break;
	case 1:	Mapper.CHR[6].n1 = What & 0xF;	break;
	case 2:	Mapper.CHR[7].n0 = What & 0xF;	break;
	case 3:	Mapper.CHR[7].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.IRQlatch.n0 = What & 0xF;	break;
	case 1:	Mapper.IRQlatch.n1 = What & 0xF;	break;
	case 2:	Mapper.IRQlatch.n2 = What & 0xF;	break;
	case 3:	Mapper.IRQlatch.n3 = What & 0xF;	break;
	}
}


static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	//if (What & 1)
			Mapper.IRQcounter = Mapper.IRQlatch.s0;
		//else	Mapper.IRQcounter = 0;
		EMU->SetIRQ(1);			break;
	case 1:	Mapper.IRQcontrol = What & 0xF;
		Sync();
		EMU->SetIRQ(1);			break;
	case 2:	Mapper.Mirror = What & 0x03;
		Sync();				break;
	case 3:					break;
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

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.PRG[0].b0 = 0x00;
	Mapper.PRG[1].b0 = 0x01;
	Mapper.PRG[2].b0 = 0xFE;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x].b0 = x;
	Mapper.IRQcontrol = 0;
	Mapper.IRQcounter = Mapper.IRQlatch.s0 = 0;
	Mapper.Mirror = 0;
	Mapper.DisableSRAM = 1;
	Sync();
}

CTMapperInfo	MapperInfo_018 =
{
	18,
	NULL,
	"Jaleco SS8806",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
