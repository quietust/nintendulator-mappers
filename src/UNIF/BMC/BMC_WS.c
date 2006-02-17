#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_ROM8(0,Mapper.Reg1 & 0x7);
	if (Mapper.Reg0 & 0x08)
	{
		EMU->SetPRG_ROM16(0x8,Mapper.Reg0 & 0x7);
		EMU->SetPRG_ROM16(0xC,Mapper.Reg0 & 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8,(Mapper.Reg0 & 0x6) >> 1);
	if (Mapper.Reg0 & 0x10)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Mapper.Reg0 & 0x20)
		return;
	switch (Addr & 1)
	{
	case 0:	Mapper.Reg0 = Val;	break;
	case 1:	Mapper.Reg1 = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x6,Write);
	Mapper.Reg0 = Mapper.Reg1 = 0;
	Sync();
}

CTMapperInfo	MapperInfo_BMC_WS =
{
	"BMC-WS",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};