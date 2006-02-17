#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Cmd, PRG, CHR, Mirror;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
	if (Mapper.Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	u16 Loc = (Bank << 12) | Addr;
	if (Loc < 0x4018)
	{
		Mapper.Write4(Bank,Addr,Val);
		return;
	}
	Val &= 0x07;
	switch (Loc & 0x4101)
	{
	case 0x4100:
		Mapper.Cmd = Val;	break;
	case 0x4101:
		switch (Mapper.Cmd)
		{
		case 0:	Mapper.PRG = 0;
			Mapper.CHR = 3;			break;
		case 2:	Mapper.CHR &= 0x7;
			Mapper.CHR |= (Val & 0x1) << 3;	break;
		case 4:	Mapper.CHR &= 0xE;
			Mapper.CHR |= Val & 0x1;	break;
		case 5:	Mapper.PRG = Val & 0x7;		break;
		case 6:	Mapper.CHR &= 0x9;
			Mapper.CHR |= (Val & 0x3) << 1;	break;
		case 7:	Mapper.Mirror = (Val & 1);	break;
		}			break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_InitROM();

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Cmd = 0;
	Mapper.PRG = 0;
	Mapper.CHR = 3;
	Mapper.Mirror = 0;

	Sync();
}

static	u8 MapperNum = 243;
CTMapperInfo	MapperInfo_243 =
{
	&MapperNum,
	"Sachen '74LS374N'",
	COMPAT_PARTIAL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};