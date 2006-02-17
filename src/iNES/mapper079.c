#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 Latch;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Mapper.Latch & 0x7);
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch & 0x8) >> 3);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	if (Addr & 0x100)
	{
		Mapper.Latch = Val;
		Sync();
	}
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);
	EMU->SetCPUWriteHandler(0x5,Write);

	if (ResetType == RESET_HARD)
		Mapper.Latch = 0;

	Sync();
}

static	u8 MapperNum = 79;
CTMapperInfo	MapperInfo_079 =
{
	&MapperNum,
	_T("NINA-03/NINA-06"),
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};