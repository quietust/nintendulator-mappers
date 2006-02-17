#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write;
	u8 CHR;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Write(Bank,Addr,Val);
	if (Addr == 0x016)
	{
		Mapper.CHR = (Val & 0x04) >> 2;
		Sync();
	}
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	Mapper.Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);

	if (ResetType == RESET_HARD)
		Mapper.CHR = 0;

	Sync();
}

static	u8 MapperNum = 99;
CTMapperInfo	MapperInfo_099 =
{
	&MapperNum,
	_T("VS Unisystem"),
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