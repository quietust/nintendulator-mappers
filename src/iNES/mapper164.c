#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	EMU->SetPRG_ROM32(0x8,Mapper.Reg);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	int addr = (Bank << 12) | Addr;
	if (Bank == 4) Mapper.Write4(Bank,Addr,Val);
	if ((addr & 0x7300) == 0x5000)
		Mapper.Reg = Val;
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 4; x < 6; x++)
		EMU->SetCPUWriteHandler(x,Write);
	for (x = 8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Reg = 0xFF;

	Sync();
}

static	u8 MapperNum = 164;
CTMapperInfo	MapperInfo_164 =
{
	&MapperNum,
	"Mapper 164",
	COMPAT_NEARLY,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};