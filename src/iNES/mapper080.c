#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[3], CHR[6];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	EMU->SetCHR_ROM2(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(2,Mapper.CHR[1]);
	EMU->SetCHR_ROM1(4,Mapper.CHR[2]);
	EMU->SetCHR_ROM1(5,Mapper.CHR[3]);
	EMU->SetCHR_ROM1(6,Mapper.CHR[4]);
	EMU->SetCHR_ROM1(7,Mapper.CHR[5]);
	if (Mapper.Mirror & 1)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	switch (Where)
	{
	case 0xEF0:	Mapper.CHR[0] = What >> 1;	break;
	case 0xEF1:	Mapper.CHR[1] = What >> 1;	break;
	case 0xEF2:	Mapper.CHR[2] = What;		break;
	case 0xEF3:	Mapper.CHR[3] = What;		break;
	case 0xEF4:	Mapper.CHR[4] = What;		break;
	case 0xEF5:	Mapper.CHR[5] = What;		break;
	case 0xEF6:	Mapper.Mirror = What;		break;
	case 0xEFA:
	case 0xEFB:	Mapper.PRG[0] = What;		break;
	case 0xEFC:
	case 0xEFD:	Mapper.PRG[1] = What;		break;
	case 0xEFE:
	case 0xEFF:	Mapper.PRG[2] = What;		break;
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

	EMU->SetCPUWriteHandler(0x7,Write);

	Mapper.Mirror = 0;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = x;
	Mapper.PRG[0] = 0;
	Mapper.PRG[1] = 1;
	Mapper.PRG[2] = -1;

	Sync();
}

CTMapperInfo	MapperInfo_080 =
{
	80,
	NULL,
	"Mapper 80",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};