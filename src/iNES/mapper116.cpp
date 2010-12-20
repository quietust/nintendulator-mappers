/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"
#include	"..\Hardware\h_MMC3.h"

namespace
{
void	Sync (void);
} // namespace

namespace VRC2
{
uint8 PRG[2];
uint8_n CHR[8];
uint8 Mirror;

void	MAPINT	CPUWrite8 (int Bank, int Addr, int Val)
{
	PRG[0] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWrite9 (int Bank, int Addr, int Val)
{
	Mirror = Val & 0xF;
	Sync();
}

void	MAPINT	CPUWriteA (int Bank, int Addr, int Val)
{
	PRG[1] = Val & 0x1F;
	Sync();
}

void	MAPINT	CPUWriteB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[0].n0 = Val & 0xF;	break;
	case 1:	CHR[1].n0 = Val & 0xF;	break;
	case 2:	CHR[0].n1 = Val & 0xF;	break;
	case 3:	CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	CPUWriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[2].n0 = Val & 0xF;	break;
	case 1:	CHR[3].n0 = Val & 0xF;	break;
	case 2:	CHR[2].n1 = Val & 0xF;	break;
	case 3:	CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	CPUWriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[4].n0 = Val & 0xF;	break;
	case 1:	CHR[5].n0 = Val & 0xF;	break;
	case 2:	CHR[4].n1 = Val & 0xF;	break;
	case 3:	CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}

void	MAPINT	CPUWriteE (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	CHR[6].n0 = Val & 0xF;	break;
	case 1:	CHR[7].n0 = Val & 0xF;	break;
	case 2:	CHR[6].n1 = Val & 0xF;	break;
	case 3:	CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}
} // namespace VRC2

namespace
{
FCPUWrite _Write4, _WriteF;
uint8 Mode;

void	Sync (void)
{
	if (Mode & 0x02)
	{
		MMC1::SyncMirror();
		MMC1::SyncPRG(0xF, 0);
		MMC1::SyncCHR_ROM(0x1F, 0);
	}
	else if (Mode & 0x01)
	{
		MMC3::SyncMirror();
		MMC3::SyncPRG(0x3F, 0);
		MMC3::SyncCHR_ROM(0xFF, (Mode & 0x04) << 6);
	}
	else
	{
		uint8 x;
		EMU->SetPRG_ROM8(0x8, VRC2::PRG[0]);
		EMU->SetPRG_ROM8(0xA, VRC2::PRG[1]);
		EMU->SetPRG_ROM16(0xC, 0xF);
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x, VRC2::CHR[x].b0 | ((Mode & 0x04) << 6));
		if (VRC2::Mirror & 0x1)
			EMU->Mirror_H();
		else	EMU->Mirror_V();
	}
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((Mode & 0x03) == 1)
		MMC3::PPUCycle(Addr, Scanline, Cycle, IsRendering);
}

void	SetMode (void)
{
	if (Mode & 0x02)
	{
		uint8 x;
		for (x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x, MMC1::Write);
		EMU->SetIRQ(1);
	}
	else if (Mode & 0x01)
	{
		EMU->SetCPUWriteHandler(0x8, MMC3::CPUWrite89);
		EMU->SetCPUWriteHandler(0x9, MMC3::CPUWrite89);
		EMU->SetCPUWriteHandler(0xA, MMC3::CPUWriteAB);
		EMU->SetCPUWriteHandler(0xB, MMC3::CPUWriteAB);
		EMU->SetCPUWriteHandler(0xC, MMC3::CPUWriteCD);
		EMU->SetCPUWriteHandler(0xD, MMC3::CPUWriteCD);
		EMU->SetCPUWriteHandler(0xE, MMC3::CPUWriteEF);
		EMU->SetCPUWriteHandler(0xF, MMC3::CPUWriteEF);
	}
	else
	{
		EMU->SetCPUWriteHandler(0x8, VRC2::CPUWrite8);
		EMU->SetCPUWriteHandler(0x9, VRC2::CPUWrite9);
		EMU->SetCPUWriteHandler(0xA, VRC2::CPUWriteA);
		EMU->SetCPUWriteHandler(0xB, VRC2::CPUWriteB);
		EMU->SetCPUWriteHandler(0xC, VRC2::CPUWriteC);
		EMU->SetCPUWriteHandler(0xD, VRC2::CPUWriteD);
		EMU->SetCPUWriteHandler(0xE, VRC2::CPUWriteE);
		EMU->SetCPUWriteHandler(0xF, _WriteF);
		EMU->SetIRQ(1);
	}
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		_Write4(Bank, Addr, Val);
	if (Addr & 0x100)
	{
		Mode = Val & 0x7;
		SetMode();
		Sync();
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	uint8 i;
	SAVELOAD_BYTE(mode, x, data, Mode);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, x, data, VRC2::PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, x, data, VRC2::CHR[i].b0);
	SAVELOAD_BYTE(mode, x, data, VRC2::Mirror);
	x += MMC3::SaveLoad(mode, x, data);
	x += MMC1::SaveLoad(mode, x, data);
	if (mode == STATE_LOAD)
	{
		SetMode();
		Sync();
	}
	return x;
}

void	MAPINT	Load (void)
{
	iNES_SetSRAM();
	MMC1::Load(Sync);
	MMC3::Load(Sync);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	uint8 x;
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	_WriteF = EMU->GetCPUWriteHandler(0xF);

	MMC1::Reset(ResetType);
	MMC3::Reset(ResetType);

	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x, Write);

	if (ResetType == RESET_HARD)
	{
		VRC2::PRG[0] = 0;	VRC2::PRG[1] = 1;
		for (x = 0; x < 8; x++)
			VRC2::CHR[x].b0 = x;
		VRC2::Mirror = 0;
		Mode = 0;
	}
	SetMode();
	Sync();
}

void	MAPINT	Unload (void)
{
	MMC1::Unload();
	MMC3::Unload();
}

uint8 MapperNum = 116;
} // namespace

const MapperInfo MapperInfo_116 =
{
	&MapperNum,
	_T("AV Girl Fighting (combination MMC1+MMC3+VRC2)"),
	COMPAT_PARTIAL,
	Load,
	Reset,
	Unload,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};