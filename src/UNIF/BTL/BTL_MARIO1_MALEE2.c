#include	"..\..\DLL\d_UNIF.h"
#include	<memory.h>

static	struct
{
	FCPURead Read;
	FCPUWrite Write;
	unsigned char ROM[0x1000];
}	Mapper;

static	int	_MAPINT	Read (int Bank, int Addr)
{
	return Mapper.Read(Bank,Addr & 0x7FF);
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Write(Bank,Addr & 0x7FF,Val);
}

static	void	_MAPINT	InitMapper (RESET_TYPE ResetType)
{
	Mapper.Read = EMU->GetCPUReadHandler(0x7);
	EMU->SetCPUReadHandler(0x7,Read);
	Mapper.Write = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x7,Write);

	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetPRG_ROM4(0x6,8);
	memcpy(&Mapper.ROM[0x000],EMU->GetPRG_Ptr4(0x6),0x800);
	memcpy(&Mapper.ROM[0x800],EMU->GetPRG_Ptr4(0x6),0x800);
	EMU->SetPRG_Ptr4(0x6,Mapper.ROM,FALSE);
	EMU->SetPRG_RAM4(0x7,0);
	EMU->SetCHR_ROM8(0,0);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_BTL_MARIO1_MALEE2 =
{
	"BTL-MARIO1-MALEE2",
	"Custom mapper for pirate SMB1",
	COMPAT_FULL,
	InitMapper,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};