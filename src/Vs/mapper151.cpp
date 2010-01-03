/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
u8 PRG[3], CHR[2];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, -1);
	EMU->SetCHR_ROM4(0, CHR[0]);
	EMU->SetCHR_ROM4(4, CHR[1]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, x, data, PRG[i]);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, CHR[i]);
	x = VS::SaveLoad(mode, x, data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val;
	Sync();
}

void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val;
	Sync();
}

void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	PRG[2] = Val;
	Sync();
}

void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	CHR[0] = Val;
	Sync();
}

void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	CHR[1] = Val;
	Sync();
}

void	MAPINT	Load (void)
{
	VS::Load();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	VS::Reset(ResetType);
	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	PRG[0] = 0;
	PRG[1] = 1;
	PRG[2] = 0xFE;
	CHR[0] = 0;
	CHR[1] = 1;

	Sync();
}

void	MAPINT	Unload (void)
{
	VS::Unload();
}

u8 MapperNum = 151;
} // namespace

CTMapperInfo	MapperInfo_151 =
{
	&MapperNum,
	_T("Extended VS Unisystem"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
};