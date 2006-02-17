#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPURead Read4;
}	Mapper;

static	int	_MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x100)
		return (~Addr & 0x3F);
	else	return Mapper.Read4(Bank,Addr);
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};