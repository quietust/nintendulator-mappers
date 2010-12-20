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
FCPUWrite _Write;
uint8 CHR;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, CHR);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, CHR);
	offset = VS::SaveLoad(mode, offset, data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	_Write(Bank, Addr, Val);
	if (Addr == 0x016)
	{
		CHR = (Val & 0x04) >> 2;
		Sync();
	}
}

void	MAPINT	Load (void)
{
	VS::Load();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->Mirror_4();
	VS::Reset(ResetType);
	_Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write);

	if (ResetType == RESET_HARD)
		CHR = 0;
	Sync();
}

void	MAPINT	Unload (void)
{
	VS::Unload();
}

uint8 MapperNum = 99;
} // namespace

const MapperInfo MapperInfo_099 =
{
	&MapperNum,
	_T("VS Unisystem"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
};