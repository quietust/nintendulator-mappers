#include	"..\DLL\d_iNES.h"

static	struct
{
	u16_n IRQcounter;
	u8 IRQenabled;
	u8 PRG[4];
	u8 CHR[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
	{
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
		EMU->SetCHR_ROM2(0 | (x << 1),Mapper.CHR[x]);
	}
}

static	int	_MAPINT	SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQenabled)
		Mapper.IRQcounter.s0++;
	if (Mapper.IRQcounter.s0 >= 896)
		EMU->SetIRQ(0);
}

static	void	_MAPINT	Write6 (int Bank, int Addr, int Val)
{
	Mapper.CHR[Addr & 3] = Val;
	Sync();
}

static	void	_MAPINT	Write7 (int Bank, int Addr, int Val)
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

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,Write6);
	EMU->SetCPUWriteHandler(0x7,Write7);

	Mapper.PRG[2] = 0xE;
	Mapper.PRG[3] = 0xF;

	Sync();
}

static	u8 MapperNum = 91;
CTMapperInfo	MapperInfo_091 =
{
	&MapperNum,
	"PC-HK-SF3",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};