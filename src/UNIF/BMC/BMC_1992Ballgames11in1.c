/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	u8 Bank, Mode;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mode & 1)
		EMU->SetPRG_ROM8(0x6,Mapper.Bank | 0x23);
	else	EMU->SetPRG_ROM8(0x6,Mapper.Bank | 0x2F);
	if (Mapper.Mode == 2)
		EMU->SetPRG_ROM16(0x8,(Mapper.Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0x8,Mapper.Bank >> 1);
	if (Mapper.Mode & 1)
		EMU->SetPRG_ROM16(0xC,(Mapper.Bank >> 1) | 1);
	else	EMU->SetPRG_ROM16(0xC,(Mapper.Bank >> 1) | 7);
	if (Mapper.Mode == 3)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Bank);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write67 (int Bank, int Addr, int Val)
{
	Mapper.Mode = ((Val & 0x10) >> 3) | ((Val & 0x02) >> 1);
	Sync();
}

static	void	MAPINT	Write89ABCDEF (int Bank, int Addr, int Val)
{
	Mapper.Bank = (Val & 0xF) << 2;
	if (Bank & 0x4)
		Mapper.Mode = (Mapper.Mode & 0x1) | ((Val & 0x10) >> 3);
	Sync();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write67);
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Write89ABCDEF);

	if (ResetType == RESET_HARD)
	{
		Mapper.Bank = 0;
		Mapper.Mode = 1;
	}
	Sync();
}

CTMapperInfo	MapperInfo_BMC_1992Ballgames11in1 =
{
	"BMC-1992Ballgames11in1",
	_T("Pirate multicart mapper"),
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
