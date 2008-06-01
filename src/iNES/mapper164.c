#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.Reg);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (!(Addr & 0x300))
	{
		Mapper.Reg = Val;
		Sync();
	}
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetCPUWriteHandler(0x5,Write);
	EMU->SetCPUWriteHandler(0xD,Write);

	if (ResetType == RESET_HARD)
		Mapper.Reg = 0xFF;

	Sync();
}

static	u8 MapperNum = 164;
CTMapperInfo	MapperInfo_164 =
{
	&MapperNum,
	_T("Mapper 164"),
	COMPAT_NEARLY,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};