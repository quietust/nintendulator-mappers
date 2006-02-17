#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRGcontrol;
	u8_n PRG[4];
	u8 CHR[8];
	u8 Mirror;
	u16 IRQcounter;
	u16_n IRQlatch;
	u8 IRQenabled;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 0; x < 3; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x].b0);
	EMU->SetPRG_ROM8(0xE,-1);
	if (Mapper.Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGcontrol)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i].b0)
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQlatch.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled & 2)
	{
		if (Mapper.IRQcounter == 0xFFFF)
		{
			Mapper.IRQcounter = Mapper.IRQlatch.s0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n0 = Val & 0xF;
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n1 = Val & 0xF;
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n2 = Val & 0xF;
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Mapper.IRQlatch.n3 = Val & 0xF;
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.IRQenabled = Val & 0x7;
	if (Mapper.IRQenabled & 0x2)
		Mapper.IRQcounter = Mapper.IRQlatch.s0;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Mapper.IRQenabled & 0x1)
		Mapper.IRQenabled |= 0x2;
	else	Mapper.IRQenabled &= ~0x2;
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.PRGcontrol = Val & 0xF;
}

static	void	_MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (Mapper.PRGcontrol)
	{
	case 0x1:	Mapper.PRG[0].n0 = Val & 0xF;	break;
	case 0x2:	Mapper.PRG[1].n0 = Val & 0xF;	break;
	case 0x3:	Mapper.PRG[2].n0 = Val & 0xF;	break;
	case 0x4:	Mapper.PRG[3].n0 = Val & 0xF;	break;
	}
	switch (Addr & 0xC00)
	{
	case 0x000:	Mapper.PRG[Addr & 3].n1 = (Val >> 4) & 1;	break;
	case 0x800:	Mapper.Mirror = Val & 1;			break;
	case 0xC00:	Mapper.CHR[Addr & 7] = Val;		break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			Mapper.PRG[x].b0 = Mapper.CHR[x | 0] = Mapper.CHR[x | 4] = 0;
		Mapper.PRGcontrol = Mapper.Mirror = 0;
	}

	Sync();
}

static	u8 MapperNum = 56;
CTMapperInfo	MapperInfo_056 =
{
	&MapperNum,
	"SMB3 Pirate",
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};