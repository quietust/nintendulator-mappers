/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

namespace
{
uint8 PRG;
uint8 CHR;
uint8 Valid;

void	Sync (void)
{
	MMC3::SyncWRAM();
	if (PRG & 0x40)
		MMC3::SyncCHR_ROM(0x7F, (CHR & 0x04) << 5);
	else	MMC3::SyncCHR_ROM(0xFF, (CHR & 0x04) << 6);
	MMC3::SyncMirror();
	if (PRG & 0x80)
	{
		if (PRG & 0x20)
			EMU->SetPRG_ROM32(0x8, (PRG >> 1) & 0x7);
		else
		{
			EMU->SetPRG_ROM16(0x8, PRG & 0xF);
			EMU->SetPRG_ROM16(0xC, PRG & 0xF);
		}
	}
	else	MMC3::SyncPRG((PRG & 0x40) ? 0x1F : 0x3F, 0);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	offset = MMC3::SaveLoad(mode, offset, data);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	SAVELOAD_BYTE(mode, offset, data, CHR);
	SAVELOAD_BYTE(mode, offset, data, Valid);
	return offset;
}

void	MAPINT	Write5 (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	PRG = Val;	break;
	case 1:	CHR = Val;	break;
	}
	Sync();
}


void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3::CPUWriteAB(Bank, 0, Val | Val >> 7);
}
void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,2,6,1,7,3,4,5};
	MMC3::CPUWrite89(Bank, 0, (Val & 0xC0) | LUT[Val & 0x7]);
	Valid = 1;
}
void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Valid)
		MMC3::CPUWrite89(Bank, 1, Val);
	Valid = 0;
}
void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Val)
	{
		MMC3::CPUWriteCD(Bank, 0, Val);
		MMC3::CPUWriteEF(Bank, 1, Val);
	}
	else	MMC3::CPUWriteEF(Bank, 0, Val);
}

BOOL	MAPINT	Load (void)
{
	MMC3::Load(Sync);
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3::Reset(ResetType);
	EMU->SetCPUWriteHandler(0x5, Write5);
	EMU->SetCPUWriteHandler(0x8, Write89);
	EMU->SetCPUWriteHandler(0x9, Write89);
	EMU->SetCPUWriteHandler(0xA, WriteAB);
	EMU->SetCPUWriteHandler(0xB, WriteAB);
	EMU->SetCPUWriteHandler(0xC, WriteCD);
	EMU->SetCPUWriteHandler(0xD, WriteCD);
	EMU->SetCPUWriteHandler(0xE, WriteEF);
	EMU->SetCPUWriteHandler(0xF, WriteEF);
	PRG = CHR = 0x00;
	Valid = 1;
	Sync();
}
void	MAPINT	Unload (void)
{
	MMC3::Unload();
}
} // namespace

const MapperInfo MapperInfo_UNL_8237 =
{
	"UNL-8237",
	_T("?"),
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3::PPUCycle,
	SaveLoad,
	NULL,
	NULL
};