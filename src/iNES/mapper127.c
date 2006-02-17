#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[4];
	u8 CHR[8];
	u8 IRQenabled;
	u8 IRQcounter;
	u8 Mirror[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,Mapper.PRG[3]);
	for (x = 0; x < 8; x++)
		EMU->SetCHR_ROM1(x,Mapper.CHR[x]);
	for (x = 0; x < 4; x++)
	{
		EMU->SetCHR_NT1(0x8 | x,Mapper.Mirror[x]);
		EMU->SetCHR_NT1(0xC | x,Mapper.Mirror[x]);
	}
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQcounter)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.Mirror[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled) && (!--Mapper.IRQcounter))
		EMU->SetIRQ(0);
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0x73)
	{
	case 0x00:	Mapper.PRG[0] = Val & 0xF;		break;
	case 0x01:	Mapper.PRG[1] = Val & 0xF;		break;
	case 0x02:	Mapper.PRG[2] = Val & 0xF;		break;
	case 0x03:	Mapper.PRG[3] = (Val & 0x3) | 0xC;	break;
	case 0x10:	Mapper.CHR[0] = Val & 0x7F;		break;
	case 0x11:	Mapper.CHR[1] = Val & 0x7F;		break;
	case 0x12:	Mapper.CHR[2] = Val & 0x7F;		break;
	case 0x13:	Mapper.CHR[3] = Val & 0x7F;		break;
	case 0x20:	Mapper.CHR[4] = Val & 0x7F;		break;
	case 0x21:	Mapper.CHR[5] = Val & 0x7F;		break;
	case 0x22:	Mapper.CHR[6] = Val & 0x7F;		break;
	case 0x23:	Mapper.CHR[7] = Val & 0x7F;		break;
	case 0x30:	case 0x31:	case 0x32:	case 0x33:
			Mapper.IRQenabled = 1;			break;
	case 0x40:	case 0x41:	case 0x42:	case 0x43:
			Mapper.IRQenabled = 0;
			Mapper.IRQcounter = 0;
			EMU->SetIRQ(1);				break;
	case 0x50:	Mapper.Mirror[0] = Val & 1;		break;
	case 0x51:	Mapper.Mirror[1] = Val & 1;		break;
	case 0x52:	Mapper.Mirror[2] = Val & 1;		break;
	case 0x53:	Mapper.Mirror[3] = Val & 1;		break;
	}
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
	{
		for (x = 0; x < 4; x++)
			Mapper.PRG[x] = 0xF;
		for (x = 0; x < 8; x++)
			Mapper.CHR[x] = 0;
		Mapper.IRQenabled = 0;
		Mapper.IRQcounter = 0;
		for (x = 0; x < 4; x++)
			Mapper.Mirror[x] = 0;
	}
	Sync();
}

static	u8 MapperNum = 127;
CTMapperInfo	MapperInfo_127 =
{
	&MapperNum,
	"Double Dragon pirate",
	COMPAT_FULL,
	NULL,
	Reset,
	NULL,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
