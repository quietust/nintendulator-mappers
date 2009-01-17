/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

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
	switch (Mapper.Mirror)
	{
	case 0:	EMU->Mirror_H();		break;
	case 1:	EMU->Mirror_V();		break;
	case 2:	EMU->Mirror_Custom(0,1,1,1);	break;
	case 3:	EMU->Mirror_S1();		break;
	}
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
		case 2:	Mapper.CHR &= 0x7;
			Mapper.CHR |= (Val & 0x1) << 3;		break;
		case 4:	Mapper.CHR &= 0xB;
			Mapper.CHR |= (Val & 0x1) << 2;		break;
		case 5:	Mapper.PRG = Val & 0x7;			break;
		case 6:	Mapper.CHR &= 0xC;
			Mapper.CHR |= Val & 0x3;		break;
		case 7:	Mapper.Mirror = (Val & 0x6) >> 1;	break;
		}			break;
	}
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x6; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
	{
		Mapper.Cmd = 0;
		Mapper.PRG = 0;
		Mapper.CHR = 3;
		Mapper.Mirror = 0;
	}

	Sync();
}

static	u8 MapperNum = 243;
CTMapperInfo	MapperInfo_243 =
{
	&MapperNum,
	_T("Sachen (74LS374N)"),
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