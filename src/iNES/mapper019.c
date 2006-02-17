#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_N106.h"

static	struct
{
	u8 PRG[4], CHR[8], NTab[4];
	u16_n IRQcounter;
	FCPURead Read4;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x] & 0x3F);
	for (x = 0; x < 8; x++)
	{
		if ((Mapper.CHR[x] < 0xE0) || (Mapper.PRG[1] & (0x40 << (x >> 2))))
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
		else	EMU->SetCHR_RAM1(x,Mapper.CHR[x] & 0x1F);
	}

	EMU->Mirror_Custom(Mapper.NTab[0] & 1,Mapper.NTab[1] & 1,Mapper.NTab[2] & 1,Mapper.NTab[3] & 1);
	for (x = 0; x < 4; x++)
	{
		if (Mapper.NTab[x] < 0xE0)
		{
			EMU->SetCHR_ROM1(x+0x8,Mapper.NTab[x]);
			EMU->SetCHR_ROM1(x+0xC,Mapper.NTab[x]);
		}
	}
	if (Mapper.PRG[0] & 0x40)
		EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter.s0)
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.NTab[i])
	x = N106sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if (Mapper.IRQcounter.b1 & 0x80)
	{
		Mapper.IRQcounter.s0++;
		if (!(Mapper.IRQcounter.b1 & 0x80))
			EMU->SetIRQ(0);
	}
}

static	int	_MAPINT	Read4 (int Bank, int Where)
{
	if (Where & 0x800)
		return N106sound_Read((Bank << 12) | Where);
	else	return Mapper.Read4(Bank,Where);
}

static	int	_MAPINT	Read5 (int Bank, int Where)
{
	EMU->SetIRQ(1);
	if (Where & 0x800)
		return Mapper.IRQcounter.b1;
	else	return Mapper.IRQcounter.b0;
}

static	void	_MAPINT	Write4 (int Bank, int Where, int What)
{
	if (Where & 0x800)
		N106sound_Write((Bank << 12) | Where,What);
	else	Mapper.Write4(Bank,Where,What);
}

static	void	_MAPINT	Write5 (int Bank, int Where, int What)
{
	EMU->SetIRQ(1);
	if (Where & 0x800)
		Mapper.IRQcounter.b1 = What;
	else	Mapper.IRQcounter.b0 = What;
}

static	void	_MAPINT	Write8 (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.CHR[1] = What;
	else	Mapper.CHR[0] = What;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.CHR[3] = What;
	else	Mapper.CHR[2] = What;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.CHR[5] = What;
	else	Mapper.CHR[4] = What;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.CHR[7] = What;
	else	Mapper.CHR[6] = What;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.NTab[1] = What;
	else	Mapper.NTab[0] = What;
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.NTab[3] = What;
	else	Mapper.NTab[2] = What;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Where, int What)
{
	if (Where & 0x800)
		Mapper.PRG[1] = What;
	else	Mapper.PRG[0] = What;
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Where, int What)
{
	if (Where & 0x800)
		N106sound_Write((Bank << 12) | Where,What);
	else	Mapper.PRG[2] = What;
	Sync();
}

static	int	_MAPINT	MapperSnd (int Cycles)
{
	return N106sound_Get(Cycles);
}

static	void	_MAPINT	Shutdown (void)
{
	N106sound_Destroy();
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	iNES_InitROM();

	Mapper.Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,Read4);
	EMU->SetCPUReadHandler(0x5,Read5);

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write4);
	EMU->SetCPUWriteHandler(0x5,Write5);
	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0x9,Write9);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xB,WriteB);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xD,WriteD);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	for (x = 0; x < 4; x++)
	{
		Mapper.PRG[x] = 0xFF;
		Mapper.CHR[x | 0] = 0xFF;
		Mapper.CHR[x | 4] = 0xFF;
		Mapper.NTab[x] = 0xFF;
	}
	Mapper.IRQcounter.s0 = 0;
	N106sound_Init();
	EMU->SetIRQ(1);
	Sync();
}

static	u8 MapperNum = 19;
CTMapperInfo	MapperInfo_019 =
{
	&MapperNum,
	"Namcot 106",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};