#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	u8 Cmd, PRG, CHR, Mirror;
	FCPUWrite Write4;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,(Mapper.PRG << 1) | 0);
	EMU->SetPRG_ROM16(0xC,(Mapper.PRG << 1) | 1);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
	if (Mapper.Mirror)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd);
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
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
	case 0x4100:	Mapper.Cmd = Val;	break;
	case 0x4101:	switch (Mapper.Cmd)
			{
			case 0:	Mapper.PRG = 0;
				Mapper.CHR = 3;				break;
			case 4:	Mapper.CHR &= 0x6;
				Mapper.CHR |= (Val & 0x1) << 0;	break;
			case 5:	Mapper.PRG = Val & 0x7;		break;
			case 6:	Mapper.CHR &= 0x1;
				Mapper.CHR |= (Val & 0x3) << 1;		break;
/*			case 4:	Mapper.CHR &= 0x3;
				Mapper.CHR |= Val << 2;	break;
			case 5:	Mapper.PRG = Val;		break;
			case 6:	Mapper.CHR &= 0x1C;
				Mapper.CHR |= Val & 0x3;	break;*/
			case 7:	Mapper.Mirror = (Val & 1);		break;
			}			break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Cmd = 0;
	Mapper.PRG = 0;
	Mapper.CHR = 3;
	Mapper.Mirror = 0;
	Sync();
}

CTMapperInfo	MapperInfo_UNL_Sachen_74LS374N =
{
	"UNL-Sachen-74LS374N",
	_T("Sachen mapper"),
	COMPAT_PARTIAL,
	NULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
