/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
uint8 PRG[3];
uint8_n CHR[2];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, 0xF);
	EMU->SetCHR_ROM4(0, CHR[0].b0);
	EMU->SetCHR_ROM4(4, CHR[1].b0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
	offset = VS::SaveLoad(mode, offset, data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val & 0xF;
	Sync();
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
// VS uses 4-screen mirroring
//	Mirror = Val & 0x1;
	CHR[0].n1 = (Val & 0x2) >> 1;
	CHR[1].n1 = (Val & 0x4) >> 2;
	Sync();
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0xF;
	Sync();
}
void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	PRG[2] = Val & 0xF;
	Sync();
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	CHR[0].n0 = Val & 0xF;
	Sync();
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	CHR[1].n0 = Val & 0xF;
	Sync();
}

BOOL	MAPINT	Load (void)
{
	VS::Load();
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	// Force 4-screen mirroring to retain compatibility with the old mapper 151
	// All VS games that use this mapper are already flagged as 4-screen VRAM, so it's okay
	EMU->Mirror_4();
	VS::Reset(ResetType);

	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		CHR[0].b0 = CHR[1].b0 = 0;
		PRG[0] = PRG[1] = PRG[2] = 0;
	}

	Sync();
}
void	MAPINT	Unload (void)
{
	VS::Unload();
}

uint8 MapperNum = 75;
uint8 MapperNum2 = 151;
} // namespace

const MapperInfo MapperInfo_075 =
{
	&MapperNum,
	_T("Konami VRC1"),
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

const MapperInfo MapperInfo_151 =
{
	&MapperNum,
	_T("Konami VRC1"),
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
