#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Mirror, VROM_use;
	u8 CHR_L, CHR_H;
	u8 PRG, CHR[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,-1);
	for (x = 0; x < 4; x++)
		EMU->SetCHR_ROM2(x << 1,Mapper.CHR[x]);
}

static	void	SyncNametables (void)
{
	switch (Mapper.Mirror & 0x3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
	if (Mapper.VROM_use)
	{
		u8 A = EMU->GetCHR_NT1(0x8) ? Mapper.CHR_H : Mapper.CHR_L;
		u8 B = EMU->GetCHR_NT1(0x9) ? Mapper.CHR_H : Mapper.CHR_L;
		u8 C = EMU->GetCHR_NT1(0xA) ? Mapper.CHR_H : Mapper.CHR_L;
		u8 D = EMU->GetCHR_NT1(0xB) ? Mapper.CHR_H : Mapper.CHR_L;
		EMU->SetCHR_ROM1(0x8,A | 0x80);
		EMU->SetCHR_ROM1(0x9,B | 0x80);
		EMU->SetCHR_ROM1(0xA,C | 0x80);
		EMU->SetCHR_ROM1(0xB,D | 0x80);
		EMU->SetCHR_ROM1(0xC,A | 0x80);
		EMU->SetCHR_ROM1(0xD,B | 0x80);
		EMU->SetCHR_ROM1(0xE,C | 0x80);
		EMU->SetCHR_ROM1(0xF,D | 0x80);
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	SAVELOAD_BYTE(mode,x,data,Mapper.VROM_use)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR_L)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR_H)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	Mapper.CHR[0] = What;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	Mapper.CHR[1] = What;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	Mapper.CHR[2] = What;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	Mapper.CHR[3] = What;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	Mapper.CHR_L = What;
	SyncNametables();
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	Mapper.CHR_H = What;
	SyncNametables();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	Mapper.Mirror = What & 0x3;
	Mapper.VROM_use = (What & 0x10);
	SyncNametables();
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	Mapper.PRG = What;
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

	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.PRG = 0;
	for (x = 0; x < 4; x++)	Mapper.CHR[x] = x;
	Mapper.Mirror = Mapper.VROM_use = 0;
	Mapper.CHR_L = Mapper.CHR_H = 0;

	Sync();
	SyncNametables();
}

static	u8 MapperNum = 68;
CTMapperInfo	MapperInfo_068 =
{
	&MapperNum,
	"SUNSOFT-4",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};