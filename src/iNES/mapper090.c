#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled, IRQcounter, IRQlatch;
	u8 Mul1, Mul2, BankMode;
	u8 PRGbanks[4];
	u16_n CHRbanks[8];
	u16_n Nametables[4];
	u8 Mirror;
}	Mapper;

static	void	SyncPRG (void)
{
	switch (Mapper.BankMode & 0x3)
	{
	case 0:	EMU->SetPRG_ROM32(0x8,-1);	break;
	case 1:	EMU->SetPRG_ROM16(0x8,Mapper.PRGbanks[1] << 1);
		EMU->SetPRG_ROM16(0xC,-1);	break;
	case 2:
		EMU->SetPRG_ROM8(0x8,Mapper.PRGbanks[0]);
		EMU->SetPRG_ROM8(0xA,Mapper.PRGbanks[1]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRGbanks[2]);
		if (Mapper.BankMode & 0x04)
			EMU->SetPRG_ROM8(0xE,Mapper.PRGbanks[3]);
		else
		{
			EMU->SetPRG_ROM8(0xE,-1);
			if (Mapper.BankMode & 0x80)
				EMU->SetPRG_ROM8(0x6,Mapper.PRGbanks[3]);
		}				break;
	case 3:	
		EMU->SetPRG_ROM8(0x8,Mapper.PRGbanks[0]);
		EMU->SetPRG_ROM8(0xA,Mapper.PRGbanks[1]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRGbanks[2]);
		EMU->SetPRG_ROM8(0xE,Mapper.PRGbanks[3]);
						break;
	}
}

static	void	SyncCHR (void)
{
	switch ((Mapper.BankMode & 0x18) >> 3)
	{
	case 0:	EMU->SetCHR_ROM8(0,Mapper.CHRbanks[0].s0);	break;
	case 1:	EMU->SetCHR_ROM4(0,Mapper.CHRbanks[0].s0);
		EMU->SetCHR_ROM4(4,Mapper.CHRbanks[4].s0);	break;
	case 2:	EMU->SetCHR_ROM2(0,Mapper.CHRbanks[0].s0);
		EMU->SetCHR_ROM2(2,Mapper.CHRbanks[2].s0);
		EMU->SetCHR_ROM2(4,Mapper.CHRbanks[4].s0);
		EMU->SetCHR_ROM2(6,Mapper.CHRbanks[6].s0);	break;
	case 3:	EMU->SetCHR_ROM1(0,Mapper.CHRbanks[0].s0);
		EMU->SetCHR_ROM1(1,Mapper.CHRbanks[1].s0);
		EMU->SetCHR_ROM1(2,Mapper.CHRbanks[2].s0);
		EMU->SetCHR_ROM1(3,Mapper.CHRbanks[3].s0);
		EMU->SetCHR_ROM1(4,Mapper.CHRbanks[4].s0);
		EMU->SetCHR_ROM1(5,Mapper.CHRbanks[5].s0);
		EMU->SetCHR_ROM1(6,Mapper.CHRbanks[6].s0);
		EMU->SetCHR_ROM1(7,Mapper.CHRbanks[7].s0);	break;
	}
}

static	void	SyncNametables (void)
{
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (Mapper.BankMode & 0x20)
	{
		if (Mapper.Nametables[0].s0 & 0x80) EMU->SetCHR_ROM8(0x8,Mapper.Nametables[0].s0);
		if (Mapper.Nametables[1].s0 & 0x80) EMU->SetCHR_ROM8(0x9,Mapper.Nametables[1].s0);
		if (Mapper.Nametables[2].s0 & 0x80) EMU->SetCHR_ROM8(0xA,Mapper.Nametables[2].s0);
		if (Mapper.Nametables[3].s0 & 0x80) EMU->SetCHR_ROM8(0xB,Mapper.Nametables[3].s0);
		if (Mapper.Nametables[0].s0 & 0x80) EMU->SetCHR_ROM8(0xC,Mapper.Nametables[0].s0);
		if (Mapper.Nametables[1].s0 & 0x80) EMU->SetCHR_ROM8(0xD,Mapper.Nametables[1].s0);
		if (Mapper.Nametables[2].s0 & 0x80) EMU->SetCHR_ROM8(0xE,Mapper.Nametables[2].s0);
		if (Mapper.Nametables[3].s0 & 0x80) EMU->SetCHR_ROM8(0xF,Mapper.Nametables[3].s0);
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mul1)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mul2)
	SAVELOAD_BYTE(mode,x,data,Mapper.BankMode)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRGbanks[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_WORD(mode,x,data,Mapper.CHRbanks[i].s0)
	for (i = 0; i < 4; i++)
		SAVELOAD_WORD(mode,x,data,Mapper.Nametables[i].s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
	{
		SyncPRG();
		SyncCHR();
		SyncNametables();
	}
	return x;
}
static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((IsRendering) && (Cycle == 262))
	{
		if ((Mapper.IRQcounter) && (!--Mapper.IRQcounter))
		{
			if (Mapper.IRQenabled)
				EMU->SetIRQ(0);
			Mapper.IRQenabled = 0;
		}
	}
}

static	int	_MAPINT	Read5 (int Bank, int Where)
{
	switch (Where)
	{
	case 0x000:	return (((Mapper.Mul1 * Mapper.Mul2) & 0x00FF) >> 0);	break;
//	case 0x001:	return (((Mapper.Mul1 * Mapper.Mul2) & 0xFF00) >> 8);	break;
	default:	return Where >> 8;
	}
}

static	void	_MAPINT	Write5 (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0x000:	Mapper.Mul1 = What;	break;
	case 0x001:	Mapper.Mul2 = What;	break;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	Mapper.PRGbanks[Where & 3] = What;
	SyncPRG();
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	Mapper.CHRbanks[Where & 7].b0 = What;
	SyncCHR();
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	Mapper.CHRbanks[Where & 7].b1 = What;
	SyncPRG();
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	if (Where & 4)
		Mapper.Nametables[Where & 3].b1 = What;
	else	Mapper.Nametables[Where & 3].b0 = What;
	SyncNametables();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	switch (Where & 0x07)
	{
	case 0:
	case 1:
	case 6:
	case 7:						break;

	case 2:	Mapper.IRQenabled = 0;			break;
	case 3:
	case 4:	if (Mapper.IRQenabled == 0)
		{
			Mapper.IRQenabled = 1;
			Mapper.IRQcounter = Mapper.IRQlatch;
		}					break;
	case 5:	if (What >= 240)
			What -= 240;
		Mapper.IRQcounter = What;
		Mapper.IRQlatch = What;			break;
	}
	EMU->SetIRQ(1);
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	switch (Where & 0x07)
	{
	case 0:	Mapper.BankMode = What;
		SyncPRG();
		SyncCHR();
		SyncNametables();	break;
	case 1:	Mapper.Mirror = What & 3;
		SyncNametables();	break;
	case 2:				break;
	case 3:				break;
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

	EMU->SetCPUReadHandler(0x5,Read5);
	EMU->SetCPUWriteHandler(0x5,Write5);
	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);

	Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch = 0;
	Mapper.Mul1 = Mapper.Mul2 = 0;
	Mapper.BankMode = 0;
	for (x = 0; x < 8; x++)	Mapper.CHRbanks[x].s0 = x;
	for (x = 0; x < 4; x++)	Mapper.Nametables[x].s0 = 0;
	Mapper.Mirror = 0;
	EMU->SetPRG_ROM32(0x8,-1);
	Mapper.PRGbanks[0] = EMU->GetPRG_ROM8(0x8);
	Mapper.PRGbanks[1] = EMU->GetPRG_ROM8(0xA);
	Mapper.PRGbanks[2] = EMU->GetPRG_ROM8(0xC);
	Mapper.PRGbanks[3] = EMU->GetPRG_ROM8(0xE);

	SyncPRG();
	SyncCHR();
	SyncNametables();
}

CTMapperInfo	MapperInfo_090 =
{
	90,
	NULL,
	"Copyright",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};