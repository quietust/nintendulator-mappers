#include	"..\DLL\d_iNES.h"

#define IRQ_CYCLES 341
static	struct
{
	u8 IRQenabled, IRQcounter;
	u8_n IRQlatch;
	s16 IRQcycles;
	u8 PRGswap;
	u8 PRG[2];
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
	switch (Mapper.Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	int i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch.b0)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcycles)
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

static	void	MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled & 2) && ((Mapper.IRQenabled & 4) || ((Mapper.IRQcycles -= 3) < 0)))
	{
		if (!(Mapper.IRQenabled & 4))
			Mapper.IRQcycles += IRQ_CYCLES;
		if (Mapper.IRQcounter == 0xFF)
		{
			Mapper.IRQcounter = Mapper.IRQlatch.b0;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.PRG[0] = Val & 0xF;
	Sync();
}

static	void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Addr |= Addr >> 2 | Addr >> 4 | Addr >> 6;
	if (Addr & 2)
		Mapper.PRGswap = Val & 0x2;
	else	Mapper.Mirror = Val & 0xF;
	Sync();
}

static	void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val & 0xF;
	Sync();
}

static	void	MAPINT	WriteB (int Bank, int Addr, int Val)
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

static	void	MAPINT	WriteC (int Bank, int Addr, int Val)
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

static	void	MAPINT	WriteD (int Bank, int Addr, int Val)
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

static	void	MAPINT	WriteE (int Bank, int Addr, int Val)
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

static	void	MAPINT	WriteF (int Bank, int Addr, int Val)
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
			Mapper.IRQcycles = IRQ_CYCLES;
		}
		EMU->SetIRQ(1);			break;
	case 3:	if (Mapper.IRQenabled & 0x1)
			Mapper.IRQenabled |= 0x2;
		else	Mapper.IRQenabled &= ~0x2;
		EMU->SetIRQ(1);			break;
	}
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
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
		for (x = 0; x < 8; x++)
			Mapper.CHR[x].b0 = x;
		Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
		Mapper.Mirror = 0;
		Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch.b0 = 0;
		Mapper.IRQcycles = 0;
	}

	Sync();
}

static	u8 MapperNum = 23;
CTMapperInfo	MapperInfo_023 =
{
	&MapperNum,
	_T("Konami VRC2/VRC4"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};