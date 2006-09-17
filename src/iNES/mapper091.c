#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[2];
	u8 CHR[4];
	u8 IRQenabled;
	u8 IRQcounter;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM2(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(2,Mapper.CHR[1]);
	EMU->SetCHR_ROM2(4,Mapper.CHR[2]);
	EMU->SetCHR_ROM2(6,Mapper.CHR[3]);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((Mapper.IRQenabled) && (Cycle == 0) && IsRendering)
	{
		Mapper.IRQcounter++;
		if (Mapper.IRQcounter >= 8)
			EMU->SetIRQ(0);
	}
}

static	void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	Mapper.CHR[Addr & 3] = Val;
	Sync();
}

static	void	MAPINT	Write7 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	Mapper.PRG[0] = Val & 0xF;	break;
	case 1:	Mapper.PRG[1] = Val & 0xF;	break;
	case 2:	Mapper.IRQenabled = 0;
		Mapper.IRQcounter = 0;
		EMU->SetIRQ(1);			break;
	case 3:	Mapper.IRQenabled = 1;		break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetCPUWriteHandler(0x6,Write6);
	EMU->SetCPUWriteHandler(0x7,Write7);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG[0] = Mapper.PRG[1] = 0;
		Mapper.CHR[0] = Mapper.CHR[1] = Mapper.CHR[2] = Mapper.CHR[3] = 0;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter = 0;
		EMU->SetIRQ(1);
	}
	Sync();
}

static	u8 MapperNum = 91;
CTMapperInfo	MapperInfo_091 =
{
	&MapperNum,
	_T("PC-HK-SF3"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};