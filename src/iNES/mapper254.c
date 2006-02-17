#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 PRG;
	u8 CHR;
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,Mapper.PRG);
	EMU->SetPRG_ROM32(0x8,3);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
	EMU->SetCHR_ROM8(0,Mapper.CHR);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
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

static	void	MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
	{
		Mapper.IRQcounter.s0++;
		if (Mapper.IRQcounter.s0 >= 24576)
			EMU->SetIRQ(0);
	}
}

static	void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.CHR = Val;
	Sync();
}

static	void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	switch (Addr & 0x3)
	{
	case 0:	Mapper.PRG = Val & 0x0F;
		Sync();				break;
	case 1:	Mapper.Mirror = Val & 0x8;
		Sync();				break;
	case 2:	if (Val & 0x02)
			Mapper.IRQenabled = 1;
		else
		{
			Mapper.IRQenabled = 0;
			Mapper.IRQcounter.s0 = 0;
			EMU->SetIRQ(1);
		}				break;
	case 3:					break;
	}
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	int x;

	EMU->SetCPUWriteHandler(0x8,Write8);
	for (x = 0xE; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,WriteEF);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = Mapper.Mirror = 0;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter.s0 = 0;
	}

	Sync();
}

static	u8 MapperNum = 254;
CTMapperInfo	MapperInfo_254 =
{
	&MapperNum,
	_T("Ai Senshi Nicol (Pirate)"),
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