/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	struct
{
	u8 Regs[4];
}	Mapper;

static	void	Sync (void)
{
	union
	{
		struct
		{
			unsigned CHRbank : 6;
			unsigned PRG16   : 1;
			unsigned PRGbank : 5;
			unsigned PRGsize : 1;
			unsigned Mir_HV  : 1;
			unsigned PRGchip : 1;
			unsigned         : 1;
		};
		u16 addr;
	}	M;
	M.addr = Latch.Addr.s0;
	EMU->SetCHR_ROM8(0,M.CHRbank);
	if (M.PRGsize)
	{
		EMU->SetPRG_ROM16(0x8,(M.PRGchip << 6) | (M.PRGbank << 1) | (M.PRG16));
		EMU->SetPRG_ROM16(0xC,(M.PRGchip << 6) | (M.PRGbank << 1) | (M.PRG16));
	}
	else	EMU->SetPRG_ROM32(0x8,(M.PRGchip << 5) | M.PRGbank);
	if (M.Mir_HV)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	x = Latch_SaveLoad_A(mode,x,data);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.Regs[i]);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	MAPINT	ReadRegs (int Bank, int Addr)
{
	if (Addr & 0x800)
		return Mapper.Regs[Addr & 3];
	else	return 0;
}

static	void	MAPINT	WriteRegs (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		Mapper.Regs[Addr & 3] = Val & 0xF;
}

static	void	MAPINT	Load (void)
{
	Latch_Load(Sync,FALSE);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUReadHandler(0x5,ReadRegs);
	EMU->SetCPUWriteHandler(0x5,WriteRegs);
	Latch_Reset(ResetType);
	if (ResetType == RESET_HARD)
	{
		Mapper.Regs[0] = Mapper.Regs[2] = 0xF;
		Mapper.Regs[1] = Mapper.Regs[3] = 0x0;
	}
}
static	void	MAPINT	Unload (void)
{
	Latch_Unload();
}

CTMapperInfo	MapperInfo_BMC_Generic115in1 =
{
	"BMC-Generic115in1",
	_T("Pirate multicart mapper"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};