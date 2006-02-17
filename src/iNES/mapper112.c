#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Cmd;
	u8 PRG[2], CHR[6];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);

	EMU->SetPRG_ROM16(0xC,-1);

	EMU->SetCHR_ROM2(0,Mapper.CHR[0] >> 1);
	EMU->SetCHR_ROM2(2,Mapper.CHR[1] >> 1);

	EMU->SetCHR_ROM1(4,Mapper.CHR[2]);
	EMU->SetCHR_ROM1(5,Mapper.CHR[3]);
	EMU->SetCHR_ROM1(6,Mapper.CHR[4]);
	EMU->SetCHR_ROM1(7,Mapper.CHR[5]);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write89 (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Mapper.Cmd = Val & 0x07;	break;
	case 1:	Mapper.Mirror = ~Val & 1;	break;
	}
	Sync();
}

static	void	_MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	switch (Mapper.Cmd)
		{
		case 0:	Mapper.PRG[0] = Val;	break;
		case 1:	Mapper.PRG[1] = Val;	break;
		case 2:	Mapper.CHR[0] = Val;	break;
		case 3:	Mapper.CHR[1] = Val;	break;
		case 4:	Mapper.CHR[2] = Val;	break;
		case 5:	Mapper.CHR[3] = Val;	break;
		case 6:	Mapper.CHR[4] = Val;	break;
		case 7:	Mapper.CHR[5] = Val;	break;
		}
	}
	Sync();
}

static	void	_MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	switch (Addr & 1)
	{
	case 0:	Mapper.Mirror = Val & 1;	break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG[0] = 0x00;
		Mapper.PRG[1] = 0x01;
		Mapper.CHR[0] = 0x00;
		Mapper.CHR[1] = 0x02;
		Mapper.CHR[2] = 0x04;
		Mapper.CHR[3] = 0x05;
		Mapper.CHR[4] = 0x06;
		Mapper.CHR[5] = 0x07;
	}

	Sync();
}

static	u8 MapperNum = 112;
CTMapperInfo	MapperInfo_112 =
{
	&MapperNum,
	_T("Mapper 112 (Asder)"),
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