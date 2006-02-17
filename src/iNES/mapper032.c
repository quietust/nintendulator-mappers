#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Byte9xxx;
	u8 PRG[3];
	u8 CHR[8];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	if (Mapper.Byte9xxx & 0x01)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Byte9xxx)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Mapper.Byte9xxx & 2)
		Mapper.PRG[2] = Val;
	else	Mapper.PRG[0] = Val;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	Mapper.Byte9xxx = Val;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	Mapper.CHR[Addr & 7] = Val;
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

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;	Mapper.PRG[2] = -1;
	for (x = 0; x < 8; x++)
		Mapper.CHR[x] = 0;
	Mapper.Byte9xxx = 0;
	Sync();
}

static	u8 MapperNum = 32;
CTMapperInfo	MapperInfo_032 =
{
	&MapperNum,
	"Irem G-101",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
