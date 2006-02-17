#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Cmd, CHRH, CHRL0, CHRL1, CHRL2, CHRL3, PRG, CHRO, Mirror;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mirror & 1)
		EMU->SetCHR_ROM8(0,(Mapper.CHRH << 3) | Mapper.CHRL0);
	else
	{
		EMU->SetCHR_ROM2(0,(Mapper.CHRH << 5) | (Mapper.CHRL0 << 2) | 0);
		EMU->SetCHR_ROM2(2,(Mapper.CHRH << 5) | (Mapper.CHRL1 << 2) | 1);
		EMU->SetCHR_ROM2(4,(Mapper.CHRH << 5) | (Mapper.CHRL2 << 2) | 2);
		EMU->SetCHR_ROM2(6,(Mapper.CHRH << 5) | (Mapper.CHRL3 << 2) | 3);
	}
	switch (Mapper.Mirror >> 1)
	{
	case 0:	EMU->Mirror_Custom(0,0,0,1);	break;
	case 1:	EMU->Mirror_H();		break;
	case 2:	EMU->Mirror_V();		break;
	case 3:	EMU->Mirror_S0();		break;
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRH)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRL0)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRL1)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRL2)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRL3)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRO)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	Val &= 0x07;
	switch (Addr & 0x101)
	{
	case 0x100:
		Mapper.Cmd = Val;	break;
	case 0x101:
		switch (Mapper.Cmd)
		{
		case 0:	Mapper.CHRL0 = Val;	break;
		case 1:	Mapper.CHRL1 = Val;	break;
		case 2:	Mapper.CHRL2 = Val;	break;
		case 3:	Mapper.CHRL3 = Val;	break;
		case 4:	Mapper.CHRH = Val;	break;
		case 5:	Mapper.PRG = Val;	break;
		case 6:	Mapper.CHRO = Val;	break;
		case 7:	Mapper.Mirror = Val;	break;
		}			break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Cmd = Mapper.CHRH = Mapper.CHRL0 = Mapper.CHRL1 = Mapper.CHRL2 = Mapper.CHRL3 = Mapper.PRG = Mapper.CHRO = Mapper.Mirror = 0;

	Sync();
}

static	u8 MapperNum = 139;
CTMapperInfo	MapperInfo_139 =
{
	&MapperNum,
	"Sachen (SA8259A)",
	COMPAT_FULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};