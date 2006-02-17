#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Reg;
	FCPURead Read4;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Reg & 0x4) >> 2);
	EMU->SetCHR_ROM8(0,Mapper.Reg & 0x3);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	_MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x100)
		return ((*EMU->OpenBus) & 0xF0) | Mapper.Reg;
	else if (Bank == 4)
		return Mapper.Read4(Bank,Addr);
	else	return -1;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	if ((Addr & 0x103) == 0x102)
		Mapper.Reg = Val & 0xF;
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	Mapper.Read4 = EMU->GetCPUReadHandler(0x4);
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
	{
		EMU->SetCPUReadHandler(x,Read);
		EMU->SetCPUWriteHandler(x,Write);
	}

	if (ResetType == RESET_HARD)
		Mapper.Reg = 0;
	Sync();
}

static	u8 MapperNum = 132;
CTMapperInfo	MapperInfo_132 =
{
	&MapperNum,
	_T("Creatom"),
	COMPAT_NEARLY,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
