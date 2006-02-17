#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG, CHR;
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,7);
	EMU->SetCHR_RAM8(0,Mapper.CHR);
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_S0();	break;
	case 1:	EMU->Mirror_S1();	break;
	case 2:	EMU->Mirror_V();	break;
	case 3:	EMU->Mirror_H();	break;
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
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

static	void	_MAPINT	Write4 (int Bank, int Where, int What)
{
	if (Where < 0x018)
	{
		Mapper.Write4(Bank,Where,What);
		return;
	}
	switch (Where)
	{
	case 0x2FC:
	case 0x2FD:
	case 0x3FE:
	case 0x2FE:	if (What & 0x10)
				Mapper.Mirror = 1;
			else	Mapper.Mirror = 0;
			Sync();				break;
	case 0x2FF:	if (What & 0x10)
				Mapper.Mirror = 3;
			else	Mapper.Mirror = 2;
			Sync();				break;
	case 0x500:	/* ? */				break;
	case 0x501:	Mapper.IRQenabled = 0;		break;
	case 0x502:	Mapper.IRQcounter.b0 = What;
			EMU->SetIRQ(1);
							break;
	case 0x503:	Mapper.IRQcounter.b1 = What;
			Mapper.IRQenabled = 1;
			EMU->SetIRQ(1);			break;
	}
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	Mapper.PRG = (What >> 2) & 0xF;
	Mapper.CHR = What & 3;
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

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write4);

	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.PRG = 0;
	Mapper.CHR = 0;
	Mapper.Mirror = 0;
	Mapper.IRQenabled = 0;
	Mapper.IRQcounter.s0 = 0;
	Sync();
}

CTMapperInfo	MapperInfo_006 =
{
	6,
	NULL,
	"FFE F4xxx",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};