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
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	if (Mapper.Mirror & 0x40)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
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

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0:	Mapper.PRG[0] = What;
		Sync();			break;
	}
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0:	Mapper.Mirror = What;
		Sync();			break;
	case 3:	Mapper.IRQenabled = What & 0x80;
		EMU->SetIRQ(1);				break;
	case 4:	Mapper.IRQcounter = Mapper.IRQlatch.s0;
		EMU->SetIRQ(1);				break;
	case 5:	Mapper.IRQlatch.b1 = What;		break;
	case 6:	Mapper.IRQlatch.b0 = What;		break;
	}
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0:	Mapper.PRG[1] = What;
		Sync();			break;
	}
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0:	Mapper.CHR[0] = What;
		Sync();			break;
	case 1:	Mapper.CHR[1] = What;
		Sync();			break;
	case 2:	Mapper.CHR[2] = What;
		Sync();			break;
	case 3:	Mapper.CHR[3] = What;
		Sync();			break;
	case 4:	Mapper.CHR[4] = What;
		Sync();			break;
	case 5:	Mapper.CHR[5] = What;
		Sync();			break;
	case 6:	Mapper.CHR[6] = What;
		Sync();			break;
	case 7:	Mapper.CHR[7] = What;
		Sync();			break;
	}
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0:	Mapper.PRG[2] = What;
		Sync();			break;
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

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -2;	Mapper.PRG[3] = -1;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = x;
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter = Mapper.IRQlatch.s0 = 0;

	Sync();
	EMU->SetIRQ(1);
}

CTMapperInfo	MapperInfo_065 =
{
	65,
	NULL,
	"Irem H-3001",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};