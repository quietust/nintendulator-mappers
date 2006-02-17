#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[2];
	u8 CHR[8];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM16(0xC,-1);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	switch (Mapper.Mirror & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S1();	break;
	case 3:	EMU->Mirror_S0();	break;
	}
}

static	int	_MAPINT	SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
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
	Mapper.Mirror = Val;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Mapper.CHR[0 | (Addr & 1)] = Val >> 1;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.CHR[2 | (Addr & 1)] = Val >> 1;
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	Mapper.CHR[4 | (Addr & 1)] = Val >> 1;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.CHR[6 | (Addr & 1)] = Val >> 1;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = x;
	Mapper.Mirror = 0;
	Sync();
}

static	u8 MapperNum = 22;
CTMapperInfo	MapperInfo_022 =
{
	&MapperNum,
	"Konami VRC2 Type A",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};