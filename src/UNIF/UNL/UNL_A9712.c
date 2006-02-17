#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
	if (Mapper.Reg0 & 0x40)
	{
		u8 BankNum = (Mapper.Reg0 & 0x05) | ((Mapper.Reg0 & 8) >> 2) | ((Mapper.Reg0 & 0x20) >> 2);
		if (Mapper.Reg0 & 0x2)
			EMU->SetPRG_ROM32(0x8,BankNum >> 1);
		else
		{
			EMU->SetPRG_ROM16(0x8,BankNum);
			EMU->SetPRG_ROM16(0xC,BankNum);
		}
	}
}

static	void	_MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		if (Addr & 1)
			Mapper.Reg1 = Val;
		else	Mapper.Reg0 = Val;
		Sync();
	}
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
	EMU->SetCPUWriteHandler(0x5,Write5);
	if (ResetType == RESET_HARD)
	{
		Mapper.Reg0 = 0;
		Mapper.Reg1 = 0;
	}
}
static	void	_MAPINT	Unload (void)
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
	MMC3_SaveLoad,
	NULL,
	NULL
};