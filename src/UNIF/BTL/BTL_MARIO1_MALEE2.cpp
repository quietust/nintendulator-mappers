/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	<memory.h>

namespace
{
FCPURead _Read;
FCPUWrite _Write;
unsigned char ROM[0x1000];

static	int	MAPINT	Read (int Bank, int Addr)
{
	return _Read(Bank, Addr & 0x7FF);
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	_Write(Bank, Addr & 0x7FF, Val);
}

static	void	MAPINT	Load (void)
{
	EMU->SetPRG_ROM4(0x6, 8);
	memcpy(&ROM[0x000], EMU->GetPRG_Ptr4(0x6), 0x800);
	memcpy(&ROM[0x800], EMU->GetPRG_Ptr4(0x6), 0x800);
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	_Read = EMU->GetCPUReadHandler(0x7);
	EMU->SetCPUReadHandler(0x7, Read);
	_Write = EMU->GetCPUWriteHandler(0x7);
	EMU->SetCPUWriteHandler(0x7, Write);

	EMU->SetPRG_Ptr4(0x6, ROM, FALSE);
	EMU->SetPRG_RAM4(0x7, 0);
	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, 0);
	UNIF_SetMirroring(NULL);
}
} // namespace

const MapperInfo MapperInfo_BTL_MARIO1_MALEE2 =
{
	"BTL-MARIO1-MALEE2",
	_T("Custom mapper for pirate SMB1"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};