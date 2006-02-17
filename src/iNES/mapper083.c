#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Mode;
	u8 PRG[5];
	u8 CHR[8];
	u16_n IRQcounter;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	if (Mapper.Mode & 0x10)
	{
		if (Mapper.Mode & 0x20)
			EMU->SetPRG_ROM8(0x6,Mapper.PRG[3]);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
		EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
		EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
		EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
		EMU->SetPRG_ROM8(0xE,0x1F);
	}
	else
	{
		if (Mapper.Mode & 0x20)
			EMU->SetPRG_ROM8(0x6,0x1F);
		else
		{
			EMU->SetPRG_OB4(0x6);
			EMU->SetPRG_OB4(0x7);
		}
		if (Mapper.Mode & 0x8)
			EMU->SetPRG_ROM16(0x8,Mapper.PRG[4]);
		else
		{
			EMU->SetPRG_ROM16(0x8,Mapper.PRG[4]);
			EMU->SetPRG_ROM16(0xC,0xF);
		}

	}
	if (ROM->INES_CHRSize > 32)
	{
		EMU->SetCHR_ROM2(0,Mapper.CHR[0]);
		EMU->SetCHR_ROM2(2,Mapper.CHR[1]);
		EMU->SetCHR_ROM2(4,Mapper.CHR[6]);
		EMU->SetCHR_ROM2(6,Mapper.CHR[7]);
	}
	else
	{
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	}
	switch (Mapper.Mode & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode)
	for (i = 0; i < 5; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.Mode & 0x80)
	{
		Mapper.IRQcounter.s0--;
		if (!Mapper.IRQcounter.s0)
			EMU->SetIRQ(0);
	}
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	switch ((Addr >> 8) & 0x3)
	{
	case 0:	Mapper.PRG[4] = Val;
		break;
	case 1:	Mapper.Mode = Val;
		break;
	case 2:	if (Addr & 1)
			Mapper.IRQcounter.b1 = Val;
		else
		{
			Mapper.IRQcounter.b0 = Val;
			EMU->SetIRQ(1);
		}
		break;
	case 3:	if (Addr & 0x10)
		{
			if (!(Addr & 0x8))
				Mapper.CHR[Addr & 0x7] = Val;
		}
		else	Mapper.PRG[Addr & 0x3] = Val & 0x1F;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x8,Write);

	if (ResetType == RESET_HARD)
	{
		Mapper.Mode = 0;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = 0;
		for (x = 0; x < 5; x++)
			Mapper.PRG[x] = 0;
		Mapper.IRQcounter.s0 = 0;
	}

	Sync();
}

static	u8 MapperNum = 83;
CTMapperInfo	MapperInfo_083 =
{
	&MapperNum,
	"Cony",
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};