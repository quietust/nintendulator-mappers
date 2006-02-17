#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 PRG;
	u8 Valid;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
	if (Mapper.PRG & 0x80)
		EMU->SetPRG_ROM16(0x8,Mapper.PRG & 0xF);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.Valid)
	x = MMC3_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write6 (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	Mapper.PRG = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3_CPUWriteAB(Bank,0,Val);
}

static	void	_MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3_CPUWrite89(Bank,0,(Val & 0xC0) | LUT[Val & 0x7]);
	Mapper.Valid = 1;
}

static	void	_MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Mapper.Valid)
		MMC3_CPUWrite89(Bank,1,Val);
	Mapper.Valid = 0;
}

static	void	_MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Val)
	{
		MMC3_CPUWriteCD(Bank,0,Val);
		MMC3_CPUWriteCD(Bank,1,Val);
		MMC3_CPUWriteEF(Bank,1,Val);
	}
	else	MMC3_CPUWriteEF(Bank,0,Val);
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
	EMU->SetCPUWriteHandler(0x6,Write6);
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = 0x00;
		Mapper.Valid = 1;
		Sync();
	}
}
static	void	_MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 114;
CTMapperInfo	MapperInfo_114 =
{
	&MapperNum,
	"Mapper 114",
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};