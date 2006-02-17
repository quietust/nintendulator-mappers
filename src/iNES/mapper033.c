#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[2];
	u8 CHR[6];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM2(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(2,Mapper.CHR[1]);
	EMU->SetCHR_ROM1(4,Mapper.CHR[2]);
	EMU->SetCHR_ROM1(5,Mapper.CHR[3]);
	EMU->SetCHR_ROM1(6,Mapper.CHR[4]);
	EMU->SetCHR_ROM1(7,Mapper.CHR[5]);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.PRG[0] = What & 0x1F;
		Mapper.Mirror = What & 0x40;	break;
	case 1:	Mapper.PRG[1] = What & 0x1F;	break;
	case 2:	Mapper.CHR[0] = What;		break;
	case 3:	Mapper.CHR[1] = What;		break;
	}
	Sync();
}

static	void	_MAPINT	WriteAB (int Bank, int Where, int What)
{
	switch (Where & 3)
	{
	case 0:	Mapper.CHR[2] = What;	break;
	case 1:	Mapper.CHR[3] = What;	break;
	case 2:	Mapper.CHR[4] = What;	break;
	case 3:	Mapper.CHR[5] = What;	break;
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

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
	for (x = 0; x < 6; x++)
		Mapper.CHR[x] = x;
	Mapper.Mirror = 0;
	Sync();
}

static	u8 MapperNum = 33;
CTMapperInfo	MapperInfo_033 =
{
	&MapperNum,
	"Taito TC0190",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
