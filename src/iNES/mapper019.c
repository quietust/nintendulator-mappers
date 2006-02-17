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
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(8 | (x << 1),Mapper.PRG[x] & 0x3F);
	for (x = 0; x < 8; x++)
	{
		if ((Mapper.CHR[x] < 0xE0) || (Mapper.PRG[1] & (0x40 << (x >> 2))))
			EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
		else	EMU->SetCHR_RAM1(x,Mapper.CHR[x] & 0x1F);
	}

	for (x = 0; x < 4; x++)
	{
		if (Mapper.NTab[x] < 0xE0)
		{
			EMU->SetCHR_ROM1(x+0x8,Mapper.NTab[x]);
			EMU->SetCHR_ROM1(x+0xC,Mapper.NTab[x]);
		}
		else
		{
			EMU->SetCHR_NT1(x+0x8,Mapper.NTab[x] & 1);
			EMU->SetCHR_NT1(x+0xC,Mapper.NTab[x] & 1);
		}
	}
	if (Mapper.PRG[0] & 0x40)
		EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
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

static	int	_MAPINT	Read4 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return N106sound_Read((Bank << 12) | Addr);
	else	return Mapper.Read4(Bank,Addr);
}

static	int	_MAPINT	Read5 (int Bank, int Addr)
{
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		return Mapper.IRQcounter.b1;
	else	return Mapper.IRQcounter.b0;
}

static	void	_MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		N106sound_Write((Bank << 12) | Addr,Val);
	else	Mapper.Write4(Bank,Addr,Val);
}

static	void	_MAPINT	Write5 (int Bank, int Addr, int Val)
{
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		Mapper.IRQcounter.b1 = Val;
	else	Mapper.IRQcounter.b0 = Val;
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.CHR[1] = Val;
	else	Mapper.CHR[0] = Val;
	Sync();
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.CHR[3] = Val;
	else	Mapper.CHR[2] = Val;
	Sync();
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.CHR[5] = Val;
	else	Mapper.CHR[4] = Val;
	Sync();
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.CHR[7] = Val;
	else	Mapper.CHR[6] = Val;
	Sync();
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.NTab[1] = Val;
	else	Mapper.NTab[0] = Val;
	Sync();
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.NTab[3] = Val;
	else	Mapper.NTab[2] = Val;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.PRG[1] = Val;
	else	Mapper.PRG[0] = Val;
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		N106sound_Write((Bank << 12) | Addr,Val);
	else	Mapper.PRG[2] = Val;
	Sync();
}

static	int	_MAPINT	MapperSnd (int Cycles)
{
	return N106sound_Get(Cycles);
}

static	void	_MAPINT	Load (void)
{
	N106sound_Load();
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

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

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
		{
			Mapper.PRG[x] = 0xFF;
			Mapper.CHR[x | 0] = 0xFF;
			Mapper.CHR[x | 4] = 0xFF;
			Mapper.NTab[x] = 0xFF;
		}
		Mapper.IRQcounter.s0 = 0;
	}
	N106sound_Reset(ResetType);
	EMU->SetIRQ(1);
	Sync();
}
static	void	_MAPINT	Unload (void)
{
	N106sound_Unload();
}

static	u8 MapperNum = 19;
CTMapperInfo	MapperInfo_019 =
{
	&MapperNum,
	_T("Namcot 106"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};