/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\Sound\s_N163.h"

namespace
{
uint8_t PRG[3], CHR[8], NTab[4];
uint16_n IRQcounter;
FCPURead _Read4, _Read6, _Read7;
FCPUWrite _Write4, _Write6, _Write7;
uint8_t SRAMprot;

enum N163FLAG {
	N163FLAG_SRAM = 0x01,	// Cartridge has battery-backed SRAM
	N163FLAG_NOSOUND = 0x02,
	N163FLAG_N175 = 0x04,
	N163FLAG_N340 = 0x08,
	N163FLAG_UNKMAP = 0x10,
	N163FLAG_SIMPLE = N163FLAG_N175 | N163FLAG_N340 | N163FLAG_UNKMAP
};
uint8_t Flags;

void	Sync (void)
{
	// All N163/N175 games with external RAM also have a battery
	if (Flags & N163FLAG_SRAM)
	{
		if (Flags & N163FLAG_N175)
		{
			// Shoehorned PRG RAM enable bit
			if (PRG[2] & 0x40)
			{
				// only 2KB - reads and writes will be mirrored below
				EMU->SetPRG_RAM4(0x6, 0);
				EMU->SetPRG_RAM4(0x7, 0);
			}
			else
			{
				EMU->SetPRG_OB4(0x6);
				EMU->SetPRG_OB4(0x7);
			}
		}
		else	EMU->SetPRG_RAM8(0x6, 0);
	}
	EMU->SetPRG_ROM8(0x8, PRG[0] & 0x3F);
	EMU->SetPRG_ROM8(0xA, PRG[1] & 0x3F);
	EMU->SetPRG_ROM8(0xC, PRG[2] & 0x3F);
	EMU->SetPRG_ROM8(0xE, 0x3F);
	for (int i = 0; i < 8; i++)
	{
		if ((CHR[i] >= 0xE0) && !(PRG[1] & ((i >= 4) ? 0x80 : 0x40)))
			EMU->SetCHR_NT1(i, CHR[i] & 1);
		else	EMU->SetCHR_ROM1(i, CHR[i]);
	}
	if (Flags & N163FLAG_N175)
		return;
	if (Flags & N163FLAG_N340)
	{
		switch ((PRG[0] & 0xC0) >> 6)
		{
		case 0:	EMU->Mirror_S0();	break;
		case 1:	EMU->Mirror_V();	break;
		case 2:	EMU->Mirror_S1();	break;
		case 3:	EMU->Mirror_H();	break;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (NTab[i] >= 0xE0)
			{
				EMU->SetCHR_NT1(0x8 | i, NTab[i] & 1);
				EMU->SetCHR_NT1(0xC | i, NTab[i] & 1);
			}
			else
			{
				EMU->SetCHR_ROM1(0x8 | i, NTab[i]);
				EMU->SetCHR_ROM1(0xC | i, NTab[i]);
			}
		}
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	if (ver == 1)
	{
		for (int i = 0; i < 3; i++)
			SAVELOAD_BYTE(mode, offset, data, PRG[i]);
		for (int i = 0; i < 8; i++)
			SAVELOAD_BYTE(mode, offset, data, CHR[i]);
		if (!(Flags & N163FLAG_SIMPLE))
		{
			SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
			for (int i = 0; i < 4; i++)
				SAVELOAD_BYTE(mode, offset, data, NTab[i]);
			SAVELOAD_BYTE(mode, offset, data, SRAMprot);
			CheckSave(offset = N163sound::SaveLoad(mode, offset, data));
		}
	}
	else
	{
		SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
		for (int i = 0; i < 3; i++)
			SAVELOAD_BYTE(mode, offset, data, PRG[i]);
		for (int i = 0; i < 8; i++)
			SAVELOAD_BYTE(mode, offset, data, CHR[i]);
		if (ROM->INES_MapperNum != 210)
			for (int i = 0; i < 4; i++)
				SAVELOAD_BYTE(mode, offset, data, NTab[i]);
		CheckSave(offset = N163sound::SaveLoad(mode, offset, data));
	}

	if (IsLoad(mode))
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQcounter.s0 & 0x8000) && ((IRQcounter.s0 & 0x7FFF) != 0x7FFF) && ((++IRQcounter.s0 & 0x7FFF) == 0x7FFF))
		EMU->SetIRQ(0);
}

// Mapper Sound
int	MAPINT	Read4 (int Bank, int Addr)
{
	if ((Addr & 0x800) && !(Flags & N163FLAG_SIMPLE))
		return N163sound::Read((Bank << 12) | Addr);
	else	return _Read4(Bank, Addr);
}
void	MAPINT	Write4 (int Bank, int Addr, int Val)
{
	if ((Addr & 0x800) && !(Flags & N163FLAG_SIMPLE))
		N163sound::Write((Bank << 12) | Addr, Val);
	else	_Write4(Bank, Addr, Val);
}

// IRQ Counter
int	MAPINT	Read5 (int Bank, int Addr)
{
	if (Flags & N163FLAG_SIMPLE)
		return -1;
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		return IRQcounter.b1;
	else	return IRQcounter.b0;
}
void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Flags & N163FLAG_SIMPLE)
		return;
	EMU->SetIRQ(1);
	if (Addr & 0x800)
		IRQcounter.b1 = Val;
	else	IRQcounter.b0 = Val;
}

// PRG RAM Read
int	MAPINT	Read6 (int Bank, int Addr)
{
	// Heuristic - N175 can have PRG RAM
	if (Flags & N163FLAG_UNKMAP)
		Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N175;

	if (Flags & N163FLAG_N175)
		return _Read6(Bank, Addr & 0x7FF);
	else	return _Read6(Bank, Addr);
}
int	MAPINT	Read7 (int Bank, int Addr)
{
	// Heuristic - N175 can have PRG RAM
	if (Flags & N163FLAG_UNKMAP)
		Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N175;

	if (Flags & N163FLAG_N175)
		return _Read7(Bank, Addr & 0x7FF);
	else	return _Read7(Bank, Addr);
}

// PRG RAM Write
void	MAPINT	Write6 (int Bank, int Addr, int Val)
{
	// Heuristic - N175 can have PRG RAM
	if (Flags & N163FLAG_UNKMAP)
		Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N175;

	if (Flags & N163FLAG_N175)
	{
		_Write6(Bank, Addr & 0x7FF, Val);
		return;
	}
	if ((SRAMprot & 0xF0) != 0x40)
		return;
	if (SRAMprot & ((Addr & 0x800) ? 0x02 : 0x01))
		return;
	_Write6(Bank, Addr, Val);
}
void	MAPINT	Write7 (int Bank, int Addr, int Val)
{
	// Heuristic - N175 can have PRG RAM
	if (Flags & N163FLAG_UNKMAP)
		Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N175;

	if (Flags & N163FLAG_N175)
	{
		_Write7(Bank, Addr & 0x7FF, Val);
		return;
	}
	if ((SRAMprot & 0xF0) != 0x40)
		return;
	if (SRAMprot & ((Addr & 0x800) ? 0x08 : 0x04))
		return;
	_Write7(Bank, Addr, Val);
}

// CHR registers
void	MAPINT	Write8 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[1] = Val;
	else	CHR[0] = Val;
	Sync();
}
void	MAPINT	Write9 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[3] = Val;
	else	CHR[2] = Val;
	Sync();
}
void	MAPINT	WriteA (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[5] = Val;
	else	CHR[4] = Val;
	Sync();
}
void	MAPINT	WriteB (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		CHR[7] = Val;
	else	CHR[6] = Val;
	Sync();
}

// Nametable registers
void	MAPINT	WriteC (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		NTab[1] = Val;
	else
	{
		// Heuristic - N175 writes here to enable PRG RAM
		if (Flags & N163FLAG_UNKMAP)
			Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N175;
		// To save on mapper state vars, store this in $F000 bit 6, which is otherwise unused
		if (Flags & N163FLAG_N175)
			PRG[2] = (PRG[2] & 0xBF) | ((Val & 1) << 6);
		NTab[0] = Val;
	}
	Sync();
}
void	MAPINT	WriteD (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		NTab[3] = Val;
	else	NTab[2] = Val;
	Sync();
}
void	MAPINT	WriteE (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		PRG[1] = Val;
	else
	{
		// Heuristic - N340 uses the upper 2 bits to set mirroring
		if ((Flags & N163FLAG_UNKMAP) && (Val & 0xE0))
			Flags = (Flags & ~N163FLAG_UNKMAP) | N163FLAG_N340;
		PRG[0] = Val;
	}
	Sync();
}
void	MAPINT	WriteF (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
	{
		SRAMprot = Val;
		if (!(Flags & N163FLAG_SIMPLE))
			N163sound::Write((Bank << 12) | Addr, Val);
	}
	else
	{
		// For N175, we store the PRG RAM Enable here
		if (Flags & N163FLAG_N175)
			PRG[2] = (Val & 0xBF) | (PRG[2] & 0x40);
		else	PRG[2] = Val;
	}
	Sync();
}

int	MAPINT	MapperSnd (int Cycles)
{
	if ((PRG[0] & 0x40) || (Flags & N163FLAG_NOSOUND))
		return 0;
	else	return N163sound::Get(Cycles);
}

// Special value to handle battery-backed internal RAM
// so that hard resets behave in a reasonable way
bool sram_loaded;
int sram_offset;
BOOL	MAPINT	Load_019 (void)
{
	sram_loaded = false;
	sram_offset = -1;
	N163sound::Load();
	Flags = 0;
	if (ROM->INES_Flags & 0x02)
	{
		// This mapper either supports 8KB or 0KB SRAM, nothing in between
		if ((ROM->INES_Version == 2) && ((ROM->INES2_PRGRAM & 0xF0) == 0))
		{
			sram_offset = 0;
			EMU->Set_SRAMSize(128);
		}
		else
		{
			sram_offset = 8192;
			EMU->Set_SRAMSize(8192 + 128);
		}
		Flags |= N163FLAG_SRAM;
	}
	if (ROM->INES_Version == 2)
		switch (ROM->INES2_SubMapper)
		{
		case 1:	
		case 2:	Flags |= N163FLAG_NOSOUND;
		case 3:	N163sound::SetVolume(2);	break;
		case 4:	N163sound::SetVolume(3);	break;
		case 5:	N163sound::SetVolume(4);	break;
		}
	return TRUE;
}
BOOL	MAPINT	Load_210 (void)
{
	sram_loaded = false;
	sram_offset = -1;
	Flags = N163FLAG_NOSOUND;
	if ((ROM->INES_Version == 2) && (ROM->INES2_SubMapper == 1))
	{
		Flags |= N163FLAG_N175;
		if (ROM->INES_Flags & 0x02)
		{
			// Family Circuit '91 requires exactly 2KB of SRAM
			EMU->Set_SRAMSize(2048);
			Flags |= N163FLAG_SRAM;
		}
	}
	else if ((ROM->INES_Version == 2) && (ROM->INES2_SubMapper == 2))
		Flags |= N163FLAG_N340;
	else	Flags |= N163FLAG_UNKMAP;
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	// If we've loaded the N163 sound state from SRAM at least once,
	// then write it back to SRAM so that it correctly persists
	if ((ResetType == RESET_HARD) && (sram_loaded))
		N163sound::SaveSRAM(sram_offset);

	_Read4 = EMU->GetCPUReadHandler(0x4);
	EMU->SetCPUReadHandler(0x4, Read4);
	EMU->SetCPUReadHandler(0x5, Read5);

	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, Write4);
	EMU->SetCPUWriteHandler(0x5, Write5);

	_Read6 = EMU->GetCPUReadHandler(0x6);
	_Read7 = EMU->GetCPUReadHandler(0x7);
	EMU->SetCPUReadHandler(0x6, Read6);
	EMU->SetCPUReadHandler(0x7, Read7);

	_Write6 = EMU->GetCPUWriteHandler(0x6);
	_Write7 = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x6, Write6);
	EMU->SetCPUWriteHandler(0x7, Write7);

	EMU->SetCPUWriteHandler(0x8, Write8);
	EMU->SetCPUWriteHandler(0x9, Write9);
	EMU->SetCPUWriteHandler(0xA, WriteA);
	EMU->SetCPUWriteHandler(0xB, WriteB);
	EMU->SetCPUWriteHandler(0xC, WriteC);
	EMU->SetCPUWriteHandler(0xD, WriteD);
	EMU->SetCPUWriteHandler(0xE, WriteE);
	EMU->SetCPUWriteHandler(0xF, WriteF);

	if (ResetType == RESET_HARD)
	{
		for (int i = 0; i < 3; i++)
			PRG[i] = 0xFF;
		for (int i = 0; i < 8; i++)
			CHR[i] = 0xFF;
		for (int i = 0; i < 4; i++)
			NTab[i] = 0xFF;
		IRQcounter.s0 = 0;
	}
	if (!(Flags & N163FLAG_SIMPLE))
	{
		N163sound::Reset(ResetType);
		// If this is our first hard reset and we have SRAM,
		// then load it into the N163 sound state
		if ((ResetType == RESET_HARD) && (sram_offset >= 0) && (!sram_loaded))
		{
			N163sound::LoadSRAM(sram_offset);
			sram_loaded = true;
		}
	}
	EMU->SetIRQ(1);
	if (Flags & (N163FLAG_N175 | N163FLAG_UNKMAP))
		iNES_SetMirroring();
	Sync();
}
void	MAPINT	Unload (void)
{
	if (!(Flags & N163FLAG_SIMPLE))
	{
		// If we've got SRAM to save, then write it out
		if (sram_offset >= 0)
			N163sound::SaveSRAM(sram_offset);
		N163sound::Unload();
	}
}

uint16_t MapperNum = 19;
uint16_t MapperNum2 = 210;
} // namespace

const MapperInfo MapperInfo_019
(
	&MapperNum,
	_T("Namcot 163"),
	COMPAT_FULL,
	Load_019,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	MapperSnd,
	NULL
);
const MapperInfo MapperInfo_210
(
	&MapperNum2,
	_T("Namcot 175/340"),
	COMPAT_FULL,
	Load_210,
	Reset,
	Unload,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
);
