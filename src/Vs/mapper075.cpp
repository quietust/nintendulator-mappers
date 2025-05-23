/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
uint8_t PRG[3];
uint8_n CHR[2];

void	Sync (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM8(0xC, PRG[2]);
	EMU->SetPRG_ROM8(0xE, 0xF);
	EMU->SetCHR_ROM4(0x0, CHR[0].b0);
	EMU->SetCHR_ROM4(0x4, CHR[1].b0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 3; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i].b0);
	CheckSave(offset = VS::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
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
	EMU->Mirror_4S();
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
} // namespace

const MapperInfo MapperInfo_075
(
	75,
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
);

const MapperInfo MapperInfo_151
(
	151,
	_T("Konami VRC1 (old mapper ID)"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS::CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS::Config
);
