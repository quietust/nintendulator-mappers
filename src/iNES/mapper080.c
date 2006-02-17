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

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
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

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0xEF0:	Mapper.CHR[0] = Val >> 1;	break;
	case 0xEF1:	Mapper.CHR[1] = Val >> 1;	break;
	case 0xEF2:	Mapper.CHR[2] = Val;		break;
	case 0xEF3:	Mapper.CHR[3] = Val;		break;
	case 0xEF4:	Mapper.CHR[4] = Val;		break;
	case 0xEF5:	Mapper.CHR[5] = Val;		break;
	case 0xEF6:	Mapper.Mirror = Val;		break;
	case 0xEFA:
	case 0xEFB:	Mapper.PRG[0] = Val;		break;
	case 0xEFC:
	case 0xEFD:	Mapper.PRG[1] = Val;		break;
	case 0xEFE:
	case 0xEFF:	Mapper.PRG[2] = Val;		break;
	}
	Sync();
}

static	void	_MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x7,Write);

	if (ResetType == RESET_HARD)
	{
		Mapper.Mirror = 0;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = x;
		Mapper.PRG[0] = 0;
		Mapper.PRG[1] = 1;
		Mapper.PRG[2] = -1;
	}

	Sync();
}

static	u8 MapperNum = 80;
CTMapperInfo	MapperInfo_080 =
{
	&MapperNum,
	"Mapper 80",
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};