/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include        "..\..\DLL\d_UNIF.h"

static  struct
{
	u8 Cmd, CHR0l, CHR1l, CHR0h, CHR1h, CHRch, PRG, CHRmode, Mirror;
	FCPUWrite Write4;
	void (*Sync) (void);
}       Mapper;

static  void    Sync_8259A (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_ROM2(0,0 | (Mapper.CHR0l << 1) | (Mapper.CHRch << 4));
	EMU->SetCHR_ROM2(2,1 | (Mapper.CHR0h << 1) | (Mapper.CHRch << 4));
	EMU->SetCHR_ROM2(4,0 | (Mapper.CHR1l << 1) | (Mapper.CHRch << 4));
	EMU->SetCHR_ROM2(6,1 | (Mapper.CHR1h << 1) | (Mapper.CHRch << 4));
	switch (Mapper.Mirror >> 1)
	{
	case 0: EMU->Mirror_Custom(0,0,0,1);     break;
	case 1: EMU->Mirror_H();                 break;
	case 2: EMU->Mirror_V();                 break;
	case 3: EMU->Mirror_S0();                break;
	}
}
static  void    Sync_8259B (void)
{
	EMU->SetPRG_ROM32(0x8,Mapper.PRG);
	EMU->SetCHR_ROM2(0,Mapper.CHR0l | (Mapper.CHRch << 3));
	EMU->SetCHR_ROM2(2,Mapper.CHR0h | (Mapper.CHRch << 3));
	EMU->SetCHR_ROM2(4,Mapper.CHR1l | (Mapper.CHRch << 3));
	EMU->SetCHR_ROM2(6,Mapper.CHR1h | (Mapper.CHRch << 3));
	switch (Mapper.Mirror >> 1)
	{
	case 0: EMU->Mirror_Custom(0,0,0,1);     break;
	case 1: EMU->Mirror_H();                 break;
	case 2: EMU->Mirror_V();                 break;
	case 3: EMU->Mirror_S0();                break;
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Cmd);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR0l);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR0h);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR1l);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR1h);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRch);
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG);
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRmode);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Mapper.Sync();
	return x;
}

static  void    MAPINT	Write (int Bank, int Addr, int Val)
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
	case 0x4100:    Mapper.Cmd = Val;      break;
	case 0x4101:    switch (Mapper.Cmd)
			{
			case 0: Mapper.CHR0l = Val;    break;
			case 1: Mapper.CHR0h = Val;    break;
			case 2: Mapper.CHR1l = Val;    break;
			case 3: Mapper.CHR1h = Val;    break;
			case 4: Mapper.CHRch = Val;    break;
			case 5: Mapper.PRG = Val;      break;
			case 6: Mapper.CHRmode = Val;  break;
			case 7: Mapper.Mirror = Val;   break;
			}                       break;
	}
	Mapper.Sync();
}

static  void    Reset (RESET_TYPE ResetType)
{
	u8 x;
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);
	Mapper.Cmd = Mapper.CHR0l = Mapper.CHR1l = Mapper.CHR0h = Mapper.CHR1h = Mapper.CHRch = Mapper.PRG = Mapper.Mirror = Mapper.CHRmode = 0;
	EMU->SetCHR_RAM8(0,0);	// in case there is no CHR ROM
	Mapper.Sync();
}
static  void    MAPINT	Reset_8259A (RESET_TYPE ResetType)
{
	Mapper.Sync = Sync_8259A;
	Reset(ResetType);
}
static  void    MAPINT	Reset_8259B (RESET_TYPE ResetType)
{
	Mapper.Sync = Sync_8259B;
	Reset(ResetType);
}

CTMapperInfo    MapperInfo_UNL_Sachen_8259A =
{
	"UNL-Sachen-8259A",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_8259A,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo    MapperInfo_UNL_Sachen_8259B =
{
	"UNL-Sachen-8259B",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_8259B,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};