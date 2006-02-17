#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0,(Mapper.Reg & 0x78) >> 3);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	if ((Addr & 0x103) == 0x102)
		Mapper.Reg = Val & 0xF;
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);
	for (x = 0xC; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
		Mapper.Reg = 0;
	Sync();
}

static	u8 MapperNum = 147;
CTMapperInfo	MapperInfo_147 =
{
	&MapperNum,
	_T("Sachen (TC-U01-1.5M)"),
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