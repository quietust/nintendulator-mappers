#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
	MMC3_SyncPRG(0x3F,0);
	if (Mapper.Reg1 == 0xAB)
		EMU->SetPRG_ROM8(0xE,0x07);
	if (Mapper.Reg1 == 0xFF)
		EMU->SetPRG_ROM8(0xE,0x09);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1);
	return x;
}

static	int	MAPINT	Read5 (int Bank, int Addr)
{
	if (Addr & 0x80)
		return -1;
	else	return 0x9F;
}

static	void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr & 0x80)
		Mapper.Reg0 = Val;
}

static	void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if ((Addr & 0x3) == 3)
	{
		Mapper.Reg1 = Val;
		Sync();
	}
	else	MMC3_CPUWrite89(Bank, Addr, Val);
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
	EMU->SetCPUReadHandler(0x5,Read5);
	EMU->SetCPUWriteHandler(0x5,Write5);
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	if (ResetType == RESET_HARD)
	{
		Mapper.Reg0 = 0;
		Mapper.Reg1 = 0;
	}
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}


CTMapperInfo	MapperInfo_UNL_A9712 =
{
	"UNL-A9712",
	_T("The Panda Prince pirate (with insane copy protection)"),
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