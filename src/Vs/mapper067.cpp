/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

namespace
{
uint8_t PRG;
uint8_t CHR[4];
uint16_n IRQcounter;
uint8_t IRQenabled;
uint8_t IRQpos;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);

	EMU->SetCHR_ROM2(0x0, CHR[0]);
	EMU->SetCHR_ROM2(0x2, CHR[1]);
	EMU->SetCHR_ROM2(0x4, CHR[2]);
	EMU->SetCHR_ROM2(0x6, CHR[3]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, IRQpos);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if (IRQenabled)
	{
		if (!IRQcounter.s0)
		{
			IRQenabled = 0;
			IRQcounter.s0 = 0xFFFF;
			EMU->SetIRQ(0);
		}
		else	IRQcounter.s0--;
	}
}

void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[0] = Val;
		Sync();
	}
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[1] = Val;
		Sync();
	}
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[2] = Val;
		Sync();
	}
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		CHR[3] = Val;
		Sync();
	}
}
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		if (IRQpos)
			IRQcounter.b0 = Val;
		else	IRQcounter.b1 = Val;
		IRQpos ^= 1;
	}
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		IRQpos = 0;
		IRQenabled = (Val & 0x10) >> 4;
		EMU->SetIRQ(1);
	}
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		PRG = Val;
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

	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	// Note: writes to $E800 are ignored
	// because Vs. does not have controllable mirroring
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		CHR[0] = 0;	CHR[1] = 0;	CHR[2] = 0;	CHR[3] = 0;
		IRQcounter.s0 = 0;
		IRQenabled = IRQpos = 0;
	}

	Sync();
}
void	MAPINT	Unload (void)
{
	VS::Unload();
}

uint16_t MapperNum = 67;
} // namespace

const MapperInfo MapperInfo_067
(
	&MapperNum,
	_T("SUNSOFT-3"),
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
