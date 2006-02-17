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

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.Valid)
	x = MMC3_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
	MMC3_Destroy();
}

static	void	_MAPINT	Write6 (int Bank, int Where, int What)
{
	switch (Where & 7)
	{
	case 0:	Mapper.PRG = What;	break;
	}
	Sync();
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	MMC3_CPUWriteAB(Bank,0,What);
}

static	void	_MAPINT	WriteAB (int Bank, int Where, int What)
{
	unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3_CPUWrite89(Bank,0,(What & 0xC0) | LUT[What & 0x7]);
	Mapper.Valid = 1;
}

static	void	_MAPINT	WriteCD (int Bank, int Where, int What)
{
	if (Mapper.Valid)
		MMC3_CPUWrite89(Bank,1,What);
	Mapper.Valid = 0;
}

static	void	_MAPINT	WriteEF (int Bank, int Where, int What)
{
	if (What)
	{
		MMC3_CPUWriteCD(Bank,0,What);
		MMC3_CPUWriteEF(Bank,1,What);
	}
	else	MMC3_CPUWriteEF(Bank,0,What);
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();
	MMC3_Init(Sync);
	EMU->SetCPUWriteHandler(0x6,Write6);
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
	Mapper.PRG = 0x00;
	Mapper.Valid = 1;
	Sync();
}

CTMapperInfo	MapperInfo_114 =
{
	114,
	NULL,
	"Mapper 114",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};