#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG, CHR[8], Mirror;
	u8 IRQenabled;
	u16_n IRQcounter;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,-1);
	for (x = 0; x < 8; x++)
	{
		if (ROM->INES_CHRSize)
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
		else	EMU->SetCHR_RAM1(x,Mapper.CHR[x] & 7);
	}
	switch (Mapper.Mirror)
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
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)

	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	_MAPINT	LoadState (int x, const char *MI)
{
	u8 i;
			Mapper.IRQcounter.b0	= MI[x++];
			Mapper.IRQcounter.b1	= MI[x++];
			Mapper.IRQenabled	= MI[x++];
			Mapper.PRG		= MI[x++];
for (i = 0; i < 8; i++)	Mapper.CHR[i]		= MI[x++];
			Mapper.Mirror		= MI[x++];
	Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		if (!Mapper.IRQcounter.s0)
			EMU->SetIRQ(0);
		Mapper.IRQcounter.s0--;
	}
}

static	int	ReadRAM (void)
{
//	EMU->DbgOut("Mapper 16 WRAM read!");
/*
== Call it in bulk ==
1. start transfer
2. address transfer
3. synchronize data
4. write data x8 in bulk
5. synchronize data
6. end transfer
*/
	return 0x00;
}

static	void	WriteRAM (int What)
{
//	EMU->DbgOut("Mapper 16 WRAM write!");
/*
== Write it in bulk ==
1. start transfer
2. address transfer
3. synchronize data
4. data write x8 in bulk synchronous data
5. data synchronize
6. end transfer
-. Delay
*/
}

static	int	_MAPINT	ReadWRAM (int Bank, int Where)
{
	if ((Where & 0xF) == 0)
		return ReadRAM();
	else	return (Bank << 4) | (Where >> 8);
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	switch (Where & 0xF)
	{
	case 0x0:	Mapper.CHR[0] = What;		break;
	case 0x1:	Mapper.CHR[1] = What;		break;
	case 0x2:	Mapper.CHR[2] = What;		break;
	case 0x3:	Mapper.CHR[3] = What;		break;
	case 0x4:	Mapper.CHR[4] = What;		break;
	case 0x5:	Mapper.CHR[5] = What;		break;
	case 0x6:	Mapper.CHR[6] = What;		break;
	case 0x7:	Mapper.CHR[7] = What;		break;
	case 0x8:	Mapper.PRG = What;		break;
	case 0x9:	Mapper.Mirror = What & 0x3;	break;
	case 0xA:	Mapper.IRQenabled = What & 1;
			EMU->SetIRQ(1);			break;
	case 0xB:	Mapper.IRQcounter.b0 = What;	break;
	case 0xC:	Mapper.IRQcounter.b1 = What;	break;
	case 0xD:	WriteRAM(What);			break;
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

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUReadHandler(x,ReadWRAM);
	for (x = 0x6; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.PRG = 0;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = x;
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;
	Sync();
}

CTMapperInfo	MapperInfo_016 =
{
	16,
	NULL,
	"Bandai",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
