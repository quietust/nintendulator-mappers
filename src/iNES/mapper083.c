#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Mirror;
	u8 PRG[4];
	u8 CHR[8];
	u8 Bank;
	u16_n IRQcounter;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x]);
	if (ROM->INES_CHRSize >= 32)
		for (x = 0; x < 4; x++)
			EMU->SetCHR_ROM2(x << 1,Mapper.CHR[(x & 2) ? (x | 6) : (x)]);
	else	for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	switch (Mapper.Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();break;
	case 3:	EMU->Mirror_S1();break;
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Bank)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQcounter.s0)
	{
		Mapper.IRQcounter.s0--;
		if (!Mapper.IRQcounter.s0)
			EMU->SetIRQ(0);
	}
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	switch (Where & 0x317)
	{
	case 0x000:	Mapper.PRG[0] = ((What << 1) & 0x1E) | 0x0;
			Mapper.PRG[1] = ((What << 1) & 0x1E) | 0x1;	break;
	case 0x100:	Mapper.Mirror = What;		break;
	case 0x200:	Mapper.IRQcounter.b0 = What;
			EMU->SetIRQ(1);			break;
	case 0x201:	Mapper.IRQcounter.b1 = What;
			EMU->SetIRQ(1);			break;
	case 0x300:	Mapper.PRG[0] = What & 0x1F;	break;
	case 0x301:	Mapper.PRG[1] = What & 0x1F;	break;
	case 0x302:	Mapper.PRG[2] = What & 0x1F;	break;
//	case 0x303:	Mapper.PRG[3] = What & 0x1F;	break;
	case 0x310:	Mapper.CHR[0] = What;	break;
	case 0x311:	Mapper.CHR[1] = What;	break;
	case 0x312:	Mapper.CHR[2] = What;	break;
	case 0x313:	Mapper.CHR[3] = What;	break;
	case 0x314:	Mapper.CHR[4] = What;	break;
	case 0x315:	Mapper.CHR[5] = What;	break;
	case 0x316:	Mapper.CHR[6] = What;	break;
	case 0x317:	Mapper.CHR[7] = What;	break;
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

	EMU->SetCPUWriteHandler(0x8,Write);

	Mapper.Mirror = 0;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = 0;
	Mapper.PRG[0] = 0x00;
	Mapper.PRG[1] = 0x01;
	Mapper.PRG[2] = 0x1E;
	Mapper.PRG[3] = 0x1F;

	Sync();
}

static	u8 MapperNum = 83;
CTMapperInfo	MapperInfo_083 =
{
	&MapperNum,
	"Cony",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};