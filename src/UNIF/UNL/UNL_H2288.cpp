/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg0, Reg1;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
	if (Mapper.Reg0 & 0x40)
	{
		u8 BankNum = (Mapper.Reg0 & 0x05) | ((Mapper.Reg0 & 8) >> 2) | ((Mapper.Reg0 & 0x20) >> 2);
		if (Mapper.Reg0 & 0x2)
			EMU->SetPRG_ROM32(0x8,BankNum >> 1);
		else
		{
			EMU->SetPRG_ROM16(0x8,BankNum);
			EMU->SetPRG_ROM16(0xC,BankNum);
		}
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1);
	return x;
}

static	int	MAPINT	Read (int Bank, int Addr)
{
	if (Addr & 0x800)
		return (*EMU->OpenBus & 0xFE) | (((~Addr >> 8) | Addr) & 1);
	else	return -1;
}

static	void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		if (Addr & 1)
			Mapper.Reg1 = Val;
		else	Mapper.Reg0 = Val;
		Sync();
	}
}

static	void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3_CPUWrite89(Bank,Addr,Val);
	else
	{
		unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3_CPUWrite89(Bank,Addr,(Val & 0xC0) | LUT[Val & 0x7]);
	}
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	MMC3_Reset(ResetType);
	EMU->SetCPUReadHandler(0x5,Read);
	EMU->SetCPUWriteHandler(0x5,Write5);
	for (x = 0x8; x < 0xA; x++)
		EMU->SetCPUWriteHandler(x,Write89);	/* need to override writes to $8000 */
	if (ResetType == RESET_HARD)
	{
		Mapper.Reg0 = 0;
		Mapper.Reg1 = 0;
	}
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}


CTMapperInfo	MapperInfo_UNL_H2288 =
{
	"UNL-H2288",
	_T("Earthworm Jim 2 Pirate (with insane copy protection)"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};