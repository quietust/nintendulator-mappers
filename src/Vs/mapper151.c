/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

static	struct
{
	u8 PRG[3], CHR[2];
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8,Mapper.PRG[0]);
	EMU->SetPRG_ROM8(0xA,Mapper.PRG[1]);
	EMU->SetPRG_ROM8(0xC,Mapper.PRG[2]);
	EMU->SetPRG_ROM8(0xE,-1);
	EMU->SetCHR_ROM4(0,Mapper.CHR[0]);
	EMU->SetCHR_ROM4(4,Mapper.CHR[1]);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i]);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i]);
	x = VS_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	Mapper.PRG[0] = Val;
	Sync();
}

static	void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	Mapper.PRG[1] = Val;
	Sync();
}

static	void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	Mapper.PRG[2] = Val;
	Sync();
}

static	void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	Mapper.CHR[0] = Val;
	Sync();
}

static	void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	Mapper.CHR[1] = Val;
	Sync();
}

static	void	MAPINT	Load (void)
{
	VS_Load();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	VS_Reset(ResetType);
	EMU->SetCPUWriteHandler(0x8,Write8);
	EMU->SetCPUWriteHandler(0xA,WriteA);
	EMU->SetCPUWriteHandler(0xC,WriteC);
	EMU->SetCPUWriteHandler(0xE,WriteE);
	EMU->SetCPUWriteHandler(0xF,WriteF);

	Mapper.PRG[0] = 0;
	Mapper.PRG[1] = 1;
	Mapper.PRG[2] = 0xFE;
	Mapper.CHR[0] = 0;
	Mapper.CHR[1] = 1;

	Sync();
}

static	void	MAPINT	Unload (void)
{
	VS_Unload();
}

static	u8 MapperNum = 151;
CTMapperInfo	MapperInfo_151 =
{
	&MapperNum,
	_T("Extended VS Unisystem"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	VS_CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS_Config
};