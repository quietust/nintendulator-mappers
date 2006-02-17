#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled, IRQcounter;
	u8_n IRQlatch;
	u8 Byte9002;
	u8 PRG[3];
	u8_n CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x].b0);
	switch (Mapper.Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S1();	break;
	case 3:	EMU->Mirror_S0();	break;
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch.b0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte9002)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i].b0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	static int counter = 341;
	counter -= 3;
	if (counter < 0)
	{
		counter += 341;
		if (Mapper.IRQenabled & 2)
		{
			if (Mapper.IRQcounter == 0xFF)
			{
				Mapper.IRQcounter = Mapper.IRQlatch.b0;
				EMU->SetIRQ(0);
			}
			else	Mapper.IRQcounter++;
		}
	}
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	if (Where == 0)
	{
		if (Mapper.Byte9002 & 2)
			Mapper.PRG[2] = What;
		else	Mapper.PRG[0] = What;
		Sync();
	}
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	if (Where == 0)
	{
		Mapper.Mirror = What;
		Sync();
	}
	else if (Where == 2)
		Mapper.Byte9002 = What;
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	if (Where == 0)
	{
		Mapper.PRG[1] = What;
		Sync();
	}
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.CHR[0].n0 = What & 0xF;	break;
	case 0x002:	Mapper.CHR[0].n1 = What & 0xF;	break;
	case 0x001:
	case 0x004:	Mapper.CHR[1].n0 = What & 0xF;	break;
	case 0x003:
	case 0x006:	Mapper.CHR[1].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.CHR[2].n0 = What & 0xF;	break;
	case 0x002:	Mapper.CHR[2].n1 = What & 0xF;	break;
	case 0x001:
	case 0x004:	Mapper.CHR[3].n0 = What & 0xF;	break;
	case 0x003:
	case 0x006:	Mapper.CHR[3].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.CHR[4].n0 = What & 0xF;	break;
	case 0x002:	Mapper.CHR[4].n1 = What & 0xF;	break;
	case 0x001:
	case 0x004:	Mapper.CHR[5].n0 = What & 0xF;	break;
	case 0x003:
	case 0x006:	Mapper.CHR[5].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.CHR[6].n0 = What & 0xF;	break;
	case 0x002:	Mapper.CHR[6].n1 = What & 0xF;	break;
	case 0x001:
	case 0x004:	Mapper.CHR[7].n0 = What & 0xF;	break;
	case 0x003:
	case 0x006:	Mapper.CHR[7].n1 = What & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.IRQlatch.n0 = What & 0xF;		break;
	case 0x002:	Mapper.IRQlatch.n1 = What & 0xF;		break;
	case 0x003:	if (Mapper.IRQenabled & 1)
				Mapper.IRQenabled |= 2;
			else	Mapper.IRQenabled &= 1;			break;
	case 0x004:	Mapper.IRQenabled = What & 3;
			if (Mapper.IRQenabled & 2)
				Mapper.IRQcounter = Mapper.IRQlatch.b0;	break;
	}
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

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch.b0 = 0;
	Mapper.Byte9002 = 0;
	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -2;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x].b0 = x;
	Sync();
}

static	u8 MapperNum = 21;
CTMapperInfo	MapperInfo_021 =
{
	&MapperNum,
	"Konami VRC4",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
