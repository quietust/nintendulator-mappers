#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled, IRQcounter;
	u8_n IRQlatch;
	u8 PRG[2];
	u8 PRGswap;
	u8_n CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM8(Mapper.PRGswap ? 0xC : 0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(Mapper.PRGswap ? 0x8 : 0xC,0x1E);
	EMU->SetPRG_ROM8(0xE,0x1F);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x].b0);
	switch (Mapper.Mirror & 1)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	int i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch.b0)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGswap)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i].b0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
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
			Mapper.IRQcounter = Mapper.IRQlatch.b0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.PRG[0] = Val & 0xF;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	if (Addr & 3)
		Mapper.PRGswap = Val & 0x2;
	else	Mapper.Mirror = Val & 0xF;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val & 0xF;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	switch (Addr & 3)
	{
	case 0:	Mapper.CHR[0].n0 = Val & 0xF;	break;
	case 1:	Mapper.CHR[0].n1 = Val & 0xF;	break;
	case 2:	Mapper.CHR[1].n0 = Val & 0xF;	break;
	case 3:	Mapper.CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	switch (Addr & 3)
	{
	case 0:	Mapper.CHR[2].n0 = Val & 0xF;	break;
	case 1:	Mapper.CHR[2].n1 = Val & 0xF;	break;
	case 2:	Mapper.CHR[3].n0 = Val & 0xF;	break;
	case 3:	Mapper.CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	switch (Addr & 3)
	{
	case 0:	Mapper.CHR[4].n0 = Val & 0xF;	break;
	case 1:	Mapper.CHR[4].n1 = Val & 0xF;	break;
	case 2:	Mapper.CHR[5].n0 = Val & 0xF;	break;
	case 3:	Mapper.CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	switch (Addr & 3)
	{
	case 0:	Mapper.CHR[6].n0 = Val & 0xF;	break;
	case 1:	Mapper.CHR[6].n1 = Val & 0xF;	break;
	case 2:	Mapper.CHR[7].n0 = Val & 0xF;	break;
	case 3:	Mapper.CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	switch (Addr & 0x3)
	{
	case 0:	Mapper.IRQlatch.n0 = Val & 0xF;	break;
	case 1:	Mapper.IRQlatch.n1 = Val & 0xF;	break;
	case 2:	Mapper.IRQenabled = Val & 0x7;
		if (Mapper.IRQenabled & 0x2)
		{
			Mapper.IRQcounter = Mapper.IRQlatch.b0;
			IRQcycles = IRQ_CYCLES;
		}
		EMU->SetIRQ(1);			break;
	case 3:	if (Mapper.IRQenabled & 0x1)
			Mapper.IRQenabled |= 0x2;
		else	Mapper.IRQenabled &= ~0x2;
		EMU->SetIRQ(1);			break;
	}
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
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

	for (x = 0; x < 8; x++)
		Mapper.CHR[x].b0 = x;
	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
	Mapper.Mirror = 0;
	Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch.b0 = 0;

	Sync();
}

static	u8 MapperNum = 23;
CTMapperInfo	MapperInfo_023 =
{
	&MapperNum,
	"Konami VRC4 Type B",
	COMPAT_PARTIAL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};