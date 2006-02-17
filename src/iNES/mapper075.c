#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[3];
	u8 CHR[2];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,0xF);
	EMU->SetCHR_ROM4(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM4(4,Mapper.CHR[1]);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.PRG[0] = Val & 0xF;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.Mirror = Val & 0x1;
	Mapper.CHR[0] = (Mapper.CHR[0] & 0xF) | ((Val & 0x2) << 3);
	Mapper.CHR[1] = (Mapper.CHR[1] & 0xF) | ((Val & 0x4) << 2);
	Sync();
}

static	void	_MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val & 0xF;
	Sync();
}

static	void	_MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	Mapper.PRG[2] = Val & 0xF;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.CHR[0] = (Mapper.CHR[0] & 0x10) | (Val & 0xF);
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Addr, int Val)
{
	Mapper.CHR[1] = (Mapper.CHR[1] & 0x10) | (Val & 0xF);
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	if (ResetType == RESET_HARD)
	{
		Mapper.CHR[0] = Mapper.CHR[1] = 0;
		Mapper.PRG[0] = Mapper.PRG[1] = Mapper.PRG[2] = 0;
		Mapper.Mirror = 0;
	}

	Sync();
}

static	u8 MapperNum = 75;
CTMapperInfo	MapperInfo_075 =
{
	&MapperNum,
	_T("Konami VRC1"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};