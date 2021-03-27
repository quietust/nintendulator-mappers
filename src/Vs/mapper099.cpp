/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
FCPUWrite _Write;
uint8_t CHR;

void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6, 0);
	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0x0, CHR);
	// Vs. Gumshoe has an extra 8KB of PRG ROM which it swaps using the same register
	if (ROM->INES_PRGSize > 2)
		EMU->SetPRG_ROM8(0x8, CHR << 2);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, CHR);
	CheckSave(offset = VS::SaveLoad(mode, offset, data));

	if (IsLoad(mode))
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

BOOL	MAPINT	Load (void)
{
	VS::Load();
	return TRUE;
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

uint16_t MapperNum = 99;
} // namespace

const MapperInfo MapperInfo_099
(
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
);
