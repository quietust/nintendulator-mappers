#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled;
	u16 IRQcounter;
	u16_n IRQlatch;
	u8 PRG[4];
	u8 CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	if (Mapper.Mirror & 0x40)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQlatch.s0)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled) && (!--Mapper.IRQcounter))
	{
		EMU->SetIRQ(0);
		Mapper.IRQenabled = 0;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	Mapper.PRG[0] = Val;
		Sync();			break;
	}
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	Mapper.Mirror = Val;
		Sync();			break;
	case 3:	Mapper.IRQenabled = Val & 0x80;
		EMU->SetIRQ(1);				break;
	case 4:	Mapper.IRQcounter = Mapper.IRQlatch.s0;
		EMU->SetIRQ(1);				break;
	case 5:	Mapper.IRQlatch.b1 = Val;		break;
	case 6:	Mapper.IRQlatch.b0 = Val;		break;
	}
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	Mapper.PRG[1] = Val;
		Sync();			break;
	}
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	Mapper.CHR[0] = Val;	break;
	case 1:	Mapper.CHR[1] = Val;	break;
	case 2:	Mapper.CHR[2] = Val;	break;
	case 3:	Mapper.CHR[3] = Val;	break;
	case 4:	Mapper.CHR[4] = Val;	break;
	case 5:	Mapper.CHR[5] = Val;	break;
	case 6:	Mapper.CHR[6] = Val;	break;
	case 7:	Mapper.CHR[7] = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0:	Mapper.PRG[2] = Val;
		Sync();			break;
	}
}

static	void	_MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -2;	Mapper.PRG[3] = -1;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = x;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter = Mapper.IRQlatch.s0 = 0;
	}

	Sync();
}

static	u8 MapperNum = 65;
CTMapperInfo	MapperInfo_065 =
{
	&MapperNum,
	_T("Irem H-3001"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};