#include	"..\DLL\d_iNES.h"

static	struct
{
	u16_n IRQcounter;
	u8 IRQenabled;
	u8 PRG[2];
	u8 CHR[4];
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[0]);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM2(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(2,Mapper.CHR[1]);
	EMU->SetCHR_ROM2(4,Mapper.CHR[2]);
	EMU->SetCHR_ROM2(6,Mapper.CHR[3]);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
		Mapper.IRQcounter.s0++;
	if (Mapper.IRQcounter.s0 >= 896)
		EMU->SetIRQ(0);
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
		Mapper.IRQcounter.s0 = 0;
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
		Mapper.IRQcounter.s0 = 0;
		Mapper.IRQenabled = 0;
		Mapper.PRG[0] = Mapper.PRG[1] = 0;
		Mapper.CHR[0] = Mapper.CHR[1] = Mapper.CHR[2] = Mapper.CHR[3] = 0;
	}
	Sync();
}

static	u8 MapperNum = 91;
CTMapperInfo	MapperInfo_091 =
{
	&MapperNum,
	_T("PC-HK-SF3"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};