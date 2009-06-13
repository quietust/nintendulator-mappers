/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPURead Read6;
	FCPUWrite Write6;
	u8 PRG[4], CHR[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
	{
		EMU->SetPRG_ROM8(0x8 | (x << 1),Mapper.PRG[x]);
		EMU->SetCHR_ROM2(x << 1,Mapper.CHR[x]);
	}
	EMU->SetPRG_RAM4(0x6,0);
}

static	int	MAPINT	SaveLoad (RESET_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i]);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	MAPINT	Read6 (int Bank, int Addr)
{
	if (Addr & 0x800)
		return Mapper.Read6(Bank,Addr & 0x7FF);
	else	return -1;
}

static	void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		Mapper.Write6(Bank,Addr & 0x7FF,Val);
		return;
	}
	switch (Addr & 0x007)
	{
	case 0:	Mapper.PRG[0] = Val;	break;
	case 1:	Mapper.PRG[1] = Val;	break;
	case 2:	Mapper.PRG[2] = Val;	break;
	case 3:	Mapper.PRG[3] = Val;	break;
	case 4:	Mapper.CHR[0] = Val;	break;
	case 5:	Mapper.CHR[1] = Val;	break;
	case 6:	Mapper.CHR[2] = Val;	break;
	case 7:	Mapper.CHR[3] = Val;	break;
	}
	Sync();
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	iNES_SetMirroring();

	Mapper.Read6 = EMU->GetCPUReadHandler(0x6);
	Mapper.Write6 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUReadHandler(0x6,Read6);
	EMU->SetCPUWriteHandler(0x6,Write6);

	if (ResetType == RESET_HARD)
	{
		Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
		Mapper.PRG[2] = 0xFE;	Mapper.PRG[3] = 0xFF;
		for (x = 0; x < 4; x++)
			Mapper.CHR[x] = 0;
	}
	Sync();
}

static	u8 MapperNum = 246;
CTMapperInfo	MapperInfo_246 =
{
	&MapperNum,
	_T("Mapper 246"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};