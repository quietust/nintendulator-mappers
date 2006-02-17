#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG[4], CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(0x8 | (x << 1),Mapper.PRG[x]);
	EMU->SetCHR_RAM8(0,0);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_V();		break;
	case 3:	EMU->Mirror_H();		break;
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	for (i = 0; i < 4; i++)
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
	if ((Mapper.IRQenabled) && (Mapper.IRQcounter.s0))
	{
		if (Mapper.IRQcounter.s0 == 0xFFFF)
			EMU->SetIRQ(0);
		Mapper.IRQcounter.s0++;
	}
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	if (Where < 0x018)
	{
		Mapper.Write4(Bank,Where,What);
		return;
	}
	switch (Where)
	{
	case 0x2FE:	if (What & 0x10)
				Mapper.Mirror = 1;
			else	Mapper.Mirror = 0;
			Sync();				break;
	case 0x2FF:	if (What & 0x10)
				Mapper.Mirror = 3;
			else	Mapper.Mirror = 2;
			Sync();				break;
	case 0x501:	Mapper.IRQenabled = 0;
			EMU->SetIRQ(1);			break;
	case 0x502:	Mapper.IRQcounter.b0 = What;
			EMU->SetIRQ(1);			break;
	case 0x503:	Mapper.IRQcounter.b1 = What;
			Mapper.IRQenabled = 1;
			EMU->SetIRQ(1);			break;
	case 0x504:	case 0x505:	case 0x506:	case 0x507:
			Mapper.PRG[Where & 3] = What;
			Sync();				break;
	case 0x510:	case 0x511:	case 0x512:	case 0x513:
	case 0x514:	case 0x515:	case 0x516:	case 0x517:
			Mapper.CHR[Where & 7] = What;
			Sync();				break;
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

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -2;	Mapper.PRG[3] = -1;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = x;
	Mapper.Mirror = 0;
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;
	Sync();
}

static	u8 MapperNum = 17;
CTMapperInfo	MapperInfo_017 =
{
	&MapperNum,
	"FFE F8xxx",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
