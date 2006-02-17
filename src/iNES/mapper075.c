#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[3];
	u8 CHR[2];
	u8 MapData;
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
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
	Mapper.PRG[0] = Val;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.Mirror = Val & 0x01;
	Mapper.CHR[0] = (Mapper.CHR[0] & 0xF) | ((Val & 0x02) << 3);
	Mapper.CHR[1] = (Mapper.CHR[1] & 0xF) | ((Val & 0x04) << 2);
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.PRG[2] = Val;
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

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.CHR[0] = 0;	Mapper.CHR[1] = 4;
	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -2;
	Mapper.Mirror = 0;

	Sync();
}

static	u8 MapperNum = 75;
CTMapperInfo	MapperInfo_075 =
{
	&MapperNum,
	"Konami VRC1",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};