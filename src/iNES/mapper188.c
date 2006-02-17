#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	if (Latch.Data & 0x10)
		EMU->SetPRG_ROM16(0x8,Latch.Data & 0x7);
	else	EMU->SetPRG_ROM16(0x8,Latch.Data | 0x8);
	EMU->SetPRG_ROM16(0xC,0x7);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	Read67 (int Bank, int Addr)
{
	return 3;
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();
	Latch_Reset(ResetType);
	EMU->SetCPUReadHandler(0x6,Read67);
	EMU->SetCPUReadHandler(0x7,Read67);
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

static	u8 MapperNum = 188;
CTMapperInfo	MapperInfo_188 =
{
	&MapperNum,
	_T("Mapper 188"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};