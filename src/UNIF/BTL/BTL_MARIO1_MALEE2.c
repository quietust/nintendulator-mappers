#include	"..\..\DLL\d_UNIF.h"
#include	<memory.h>

static	struct
{
	FCPURead ReadWRAM;
	FCPUWrite WriteWRAM;
}	Mapper;

static	void	_MAPINT	WriteROM (int Bank, int Where, int What)
{
}

static	int	_MAPINT	ReadWRAM (int Bank, int Where)
{
	return Mapper.ReadWRAM(Bank,Where & 0x7FF);
}

static	void	_MAPINT	WriteWRAM (int Bank, int Where, int What)
{
	Mapper.WriteWRAM(Bank,Where & 0x7FF,What);
}

static	void	_MAPINT	InitMapper (int IsHardReset)
{
	Mapper.ReadWRAM = EMU->GetCPUReadHandler(0x7);
	Mapper.WriteWRAM = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUReadHandler(0x7,ReadWRAM);
	EMU->SetCPUWriteHandler(0x7,WriteWRAM);

	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetPRG_ROM4(0x6,8);
	EMU->SetPRG_RAM4(0x7,1);
	memcpy(EMU->GetPRG_Ptr4(0x7),EMU->GetPRG_Ptr4(0x6),0x800);
	memcpy(EMU->GetPRG_Ptr4(0x7) + 0x800,EMU->GetPRG_Ptr4(0x6),0x800);
	EMU->SetPRG_RAM4(0x6,1);
	EMU->SetCPUWriteHandler(0x6,WriteROM);
	EMU->SetPRG_RAM4(0x7,0);
	EMU->SetCHR_ROM8(0,0);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_BTL_MARIO1_MALEE2 =
{
	-1,
	"BTL-MARIO1-MALEE2",
	"BTL-MARIO1-MALEE2",
	COMPAT_FULL,
	InitMapper,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};