#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRGcontrol;
	u8 PRGlow[4], PRGhigh[4];
	u8 CHR[8];
	u8 Mirror;
	u16_n IRQcounter;
	u8 IRQenabled;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x6,Mapper.PRGlow[3] | Mapper.PRGhigh[3]);
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRGlow[x] | Mapper.PRGhigh[x]);
	EMU->SetPRG_ROM8(0xE,-1);
	if (Mapper.Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGcontrol)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRGlow[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRGhigh[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
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
		if (!Mapper.IRQcounter.s0)
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
	EMU->SetIRQ(1);
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n1 = What & 0xF;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n2 = What & 0xF;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	Mapper.IRQcounter.n3 = What & 0xF;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	Mapper.IRQenabled = What & 0xF;
//	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	Mapper.PRGcontrol = What & 0xF;
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	switch (Mapper.PRGcontrol)
	{
	case 0x1:	Mapper.PRGlow[0] = What & 0xF;	break;
	case 0x2:	Mapper.PRGlow[1] = What & 0xF;	break;
	case 0x3:	Mapper.PRGlow[2] = What & 0xF;	break;
	case 0x4:	Mapper.PRGlow[3] = What & 0xF;	break;
	}
	switch (Where & 0xC00)
	{
	case 0x000:	Mapper.PRGhigh[Where & 0x03] = What & 0x10;	break;
	case 0x800:	Mapper.Mirror = What & 0x01;			break;
	case 0xC00:	Mapper.CHR[Where & 0x07] = What;		break;
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
		Mapper.PRGlow[x] = Mapper.PRGhigh[x] = Mapper.CHR[x | 0] = Mapper.CHR[x | 4] = 0;
	Mapper.PRGcontrol = Mapper.Mirror = 0;

	Sync();
}

static	u8 MapperNum = 56;
CTMapperInfo	MapperInfo_056 =
{
	&MapperNum,
	"SMB3 Pirate",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};