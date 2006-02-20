#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Mode;
	u8 PRG;
	u8 CHR[2];
	FCPUWrite Write7;
}	Mapper;

static	void	Sync_NINA (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_ROM4(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM4(4,Mapper.CHR[1]);
}

static	void	Sync_BNROM (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_RAM8(0,0);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	if (Mapper.Mode == 1)
	{
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[0])
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[1])
	}
	if (mode == STATE_LOAD)
	{
		if (Mapper.Mode == 1)
			Sync_NINA();
		if (Mapper.Mode == 2)
			Sync_BNROM();
	}
	return x;
}

static	void	MAPINT	WriteNINA (int Bank, int Addr, int Val)
{
	Mapper.Write7(Bank,Addr,Val);
	switch (Addr)
	{
	case 0xFFD:	Mapper.PRG = Val;
			Sync_NINA();		break;
	case 0xFFE:	Mapper.CHR[0] = Val;
			Sync_NINA();		break;
	case 0xFFF:	Mapper.CHR[1] = Val;
			Sync_NINA();		break;
	}
}

static	void	MAPINT	WriteBNROM (int Bank, int Addr, int Val)
{
	Mapper.PRG = Val;
	Sync_BNROM();
}

static	void	MAPINT	Load (void)
{
	if (ROM->INES_CHRSize == 0)
		Mapper.Mode = 2;
	else	Mapper.Mode = 1;
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG = 0;
		Mapper.CHR[0] = 0;
		Mapper.CHR[1] = 1;
	}

	if (Mapper.Mode == 1)
	{
		Mapper.Write7 = EMU->GetCPUWriteHandler(0x7);
		EMU->SetCPUWriteHandler(0x7,WriteNINA);
		Sync_NINA();
	}
	if (Mapper.Mode == 2)
	{
		for (x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x,WriteBNROM);
		Sync_BNROM();
	}

}

static	u8 MapperNum = 34;
CTMapperInfo	MapperInfo_034 =
{
	&MapperNum,
	_T("BNROM/Nina-01"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
