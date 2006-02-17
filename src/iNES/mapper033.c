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

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
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

static	void	_MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	Mapper.PRG[0] = Val & 0x1F;
		Mapper.Mirror = Val & 0x40;	break;
	case 1:	Mapper.PRG[1] = Val & 0x1F;	break;
	case 2:	Mapper.CHR[0] = Val;		break;
	case 3:	Mapper.CHR[1] = Val;		break;
	}
	Sync();
}

static	void	_MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	Mapper.CHR[2] = Val;	break;
	case 1:	Mapper.CHR[3] = Val;	break;
	case 2:	Mapper.CHR[4] = Val;	break;
	case 3:	Mapper.CHR[5] = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
		for (x = 0; x < 6; x++)
			Mapper.CHR[x] = x;
		Mapper.Mirror = 0;
	}
	Sync();
}

static	u8 MapperNum = 33;
CTMapperInfo	MapperInfo_033 =
{
	&MapperNum,
	_T("Taito TC0190"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
