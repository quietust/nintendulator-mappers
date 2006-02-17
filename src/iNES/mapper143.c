#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPURead Read4;
}	Mapper;

static	int	_MAPINT	Read (int Bank, int Where)
{
	if (Where & 0x100)
		return (~Where & 0x3F);
	else	return Mapper.Read4(Bank,Where);
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	Mapper.Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4,Read);

	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,0);
}

static	u8 MapperNum = 143;
CTMapperInfo	MapperInfo_143 =
{
	&MapperNum,
	"Sachen Pathetic Copy-Protection",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};