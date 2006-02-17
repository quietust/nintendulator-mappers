#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_VRC6.h"

#define IRQ_CYCLES 341
static	struct
{
	u8 IRQenabled, IRQcounter, IRQlatch;
	int IRQcycles;
	u8 PRG[2], CHR[8];
	u8 Mirror;
	u8 SwapAddr[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM16(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xE,-1);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	switch ((Mapper.Mirror >> 2) & 3)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcycles)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	x = VRC6sound_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled & 2) && ((Mapper.IRQenabled & 4) || ((Mapper.IRQcycles -= 3) < 0)))
	{
		if (!(Mapper.IRQenabled & 4))
			Mapper.IRQcycles += IRQ_CYCLES;
		if (Mapper.IRQcounter == 0xFF)
		{
			Mapper.IRQcounter = Mapper.IRQlatch;
			EMU->SetIRQ(0);
		}
		else	Mapper.IRQcounter++;
	}
}

static	void	_MAPINT	Write8 (int Bank, int Addr, int Val)
{
	switch (Mapper.SwapAddr[Addr & 3])
	{
	case 0:	Mapper.PRG[0] = Val;
		Sync();			break;
	case 1:				break;
	case 2:				break;
	case 3:				break;
	}
}

static	void	_MAPINT	Write9 (int Bank, int Addr, int Val)
{
	VRC6sound_Write(0x9000 | Mapper.SwapAddr[Addr & 3],Val);
}

static	void	_MAPINT	WriteA (int Bank, int Addr, int Val)
{
	VRC6sound_Write(0xA000 | Mapper.SwapAddr[Addr & 3],Val);
}

static	void	_MAPINT	WriteB (int Bank, int Addr, int Val)
{
	switch (Mapper.SwapAddr[Addr & 3])
	{
	case 0:	case 1:	case 2:
		VRC6sound_Write(0xB000 | Mapper.SwapAddr[Addr & 3],Val);
					break;
	case 3:	Mapper.Mirror = Val;
		Sync();			break;
	}
	
}

static	void	_MAPINT	WriteC (int Bank, int Addr, int Val)
{
	switch (Mapper.SwapAddr[Addr & 3])
	{
	case 0:	Mapper.PRG[1] = Val;
		Sync();			break;
	case 1:				break;
	case 2:				break;
	case 3:				break;
	}
}

static	void	_MAPINT	WriteD (int Bank, int Addr, int Val)
{
	Mapper.CHR[Mapper.SwapAddr[Addr & 3]] = Val;
	Sync();
}

static	void	_MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.CHR[4 | Mapper.SwapAddr[Addr & 3]] = Val;
	Sync();
}

static	void	_MAPINT	WriteF (int Bank, int Addr, int Val)
{
	switch (Mapper.SwapAddr[Addr & 3])
	{
	case 0:	Mapper.IRQlatch = Val;	break;
	case 1:	Mapper.IRQenabled = Val & 0x7;
		if (Mapper.IRQenabled & 0x2)
		{
			Mapper.IRQcounter = Mapper.IRQlatch;
			Mapper.IRQcycles = IRQ_CYCLES;
		}
		EMU->SetIRQ(1);
		break;
	case 2:	if (Mapper.IRQenabled & 0x1)
			Mapper.IRQenabled |= 0x2;
		else	Mapper.IRQenabled &= ~0x2;
		EMU->SetIRQ(1);		break;
	case 3:				break;
	}
}

static	int	_MAPINT	MapperSnd (int Cycles)
{
	return VRC6sound_Get(Cycles);
}

static	void	_MAPINT	Load_024 (void)
{
	VRC6sound_Load();
	Mapper.SwapAddr[0] = 0;
	Mapper.SwapAddr[1] = 1;
	Mapper.SwapAddr[2] = 2;
	Mapper.SwapAddr[3] = 3;
	iNES_SetSRAM();
}
static	void	_MAPINT	Load_026 (void)
{
	VRC6sound_Load();
	Mapper.SwapAddr[0] = 0;
	Mapper.SwapAddr[1] = 2;
	Mapper.SwapAddr[2] = 1;
	Mapper.SwapAddr[3] = 3;
	iNES_SetSRAM();
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

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
		Mapper.PRG[0] = 0;
		Mapper.PRG[1] = -2;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = x;
		Mapper.IRQenabled = Mapper.IRQcounter = Mapper.IRQlatch = 0;
		Mapper.IRQcycles = 0;
	}

	VRC6sound_Reset(ResetType);
	Sync();
}
static	void	_MAPINT	Unload (void)
{
	VRC6sound_Unload();
}

static	u8 MapperNum = 24;
CTMapperInfo	MapperInfo_024 =
{
	&MapperNum,
	"Konami VRC6 A0/A1",
	COMPAT_FULL,
	Load_024,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};

static	u8 MapperNum2 = 26;
CTMapperInfo	MapperInfo_026 =
{
	&MapperNum2,
	"Konami VRC6 A1/A0",
	COMPAT_FULL,
	Load_026,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
};
