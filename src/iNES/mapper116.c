/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC1.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 PRG[2];
	u8_n CHR[8];
	u8 Mirror;
}	VRC2;

static	struct
{
	FCPUWrite Write4, WriteF;
	u8 Mode;
}	Mapper;

static	void	Sync (void)
{
	if (Mapper.Mode & 0x02)
	{

		MMC1_SyncMirror();
		MMC1_SyncPRG(0xF,0);
		MMC1_SyncCHR_ROM(0x1F,0);
	}
	else if (Mapper.Mode & 0x01)
	{
		MMC3_SyncMirror();
		MMC3_SyncPRG(0x3F,0);
		MMC3_SyncCHR_ROM(0xFF,(Mapper.Mode & 0x04) << 6);
	}
	else
	{
		u8 x;
		EMU->SetPRG_ROM8(0x8,VRC2.PRG[0]);
		EMU->SetPRG_ROM8(0xA,VRC2.PRG[1]);
		EMU->SetPRG_ROM16(0xC,0xF);
		for (x = 0; x < 8; x++)
			EMU->SetCHR_ROM1(x,VRC2.CHR[x].b0 | ((Mapper.Mode & 0x04) << 6));
		if (VRC2.Mirror & 0x1)
			EMU->Mirror_H();
		else	EMU->Mirror_V();
	}
}

void	MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	if ((Mapper.Mode & 0x03) == 1)
		MMC3_PPUCycle(Addr, Scanline, Cycle, IsRendering);
}

static	void	MAPINT	VRC2_CPUWrite8 (int Bank, int Addr, int Val)
{
	VRC2.PRG[0] = Val & 0x1F;
	Sync();
}

static	void	MAPINT	VRC2_CPUWrite9 (int Bank, int Addr, int Val)
{
	VRC2.Mirror = Val & 0xF;
	Sync();
}

static	void	MAPINT	VRC2_CPUWriteA (int Bank, int Addr, int Val)
{
	VRC2.PRG[1] = Val & 0x1F;
	Sync();
}

static	void	MAPINT	VRC2_CPUWriteB (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	VRC2.CHR[0].n0 = Val & 0xF;	break;
	case 1:	VRC2.CHR[1].n0 = Val & 0xF;	break;
	case 2:	VRC2.CHR[0].n1 = Val & 0xF;	break;
	case 3:	VRC2.CHR[1].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	MAPINT	VRC2_CPUWriteC (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	VRC2.CHR[2].n0 = Val & 0xF;	break;
	case 1:	VRC2.CHR[3].n0 = Val & 0xF;	break;
	case 2:	VRC2.CHR[2].n1 = Val & 0xF;	break;
	case 3:	VRC2.CHR[3].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	MAPINT	VRC2_CPUWriteD (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	VRC2.CHR[4].n0 = Val & 0xF;	break;
	case 1:	VRC2.CHR[5].n0 = Val & 0xF;	break;
	case 2:	VRC2.CHR[4].n1 = Val & 0xF;	break;
	case 3:	VRC2.CHR[5].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	MAPINT	VRC2_CPUWriteE (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	VRC2.CHR[6].n0 = Val & 0xF;	break;
	case 1:	VRC2.CHR[7].n0 = Val & 0xF;	break;
	case 2:	VRC2.CHR[6].n1 = Val & 0xF;	break;
	case 3:	VRC2.CHR[7].n1 = Val & 0xF;	break;
	}
	Sync();
}

static	void	SetMode (void)
{
	if (Mapper.Mode & 0x02)
	{
		u8 x;
		for (x = 0x8; x < 0x10; x++)
			EMU->SetCPUWriteHandler(x,MMC1_Write);
		EMU->SetIRQ(1);
	}
	else if (Mapper.Mode & 0x01)
	{
		EMU->SetCPUWriteHandler(0x8,MMC3_CPUWrite89);
		EMU->SetCPUWriteHandler(0x9,MMC3_CPUWrite89);
		EMU->SetCPUWriteHandler(0xA,MMC3_CPUWriteAB);
		EMU->SetCPUWriteHandler(0xB,MMC3_CPUWriteAB);
		EMU->SetCPUWriteHandler(0xC,MMC3_CPUWriteCD);
		EMU->SetCPUWriteHandler(0xD,MMC3_CPUWriteCD);
		EMU->SetCPUWriteHandler(0xE,MMC3_CPUWriteEF);
		EMU->SetCPUWriteHandler(0xF,MMC3_CPUWriteEF);
	}
	else
	{
		EMU->SetCPUWriteHandler(0x8,VRC2_CPUWrite8);
		EMU->SetCPUWriteHandler(0x9,VRC2_CPUWrite9);
		EMU->SetCPUWriteHandler(0xA,VRC2_CPUWriteA);
		EMU->SetCPUWriteHandler(0xB,VRC2_CPUWriteB);
		EMU->SetCPUWriteHandler(0xC,VRC2_CPUWriteC);
		EMU->SetCPUWriteHandler(0xD,VRC2_CPUWriteD);
		EMU->SetCPUWriteHandler(0xE,VRC2_CPUWriteE);
		EMU->SetCPUWriteHandler(0xF,Mapper.WriteF);
		EMU->SetIRQ(1);
	}
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Bank == 4)
		Mapper.Write4(Bank,Addr,Val);
	if (Addr & 0x100)
	{
		Mapper.Mode = Val & 0x7;
		SetMode();
		Sync();
	}
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,Mapper.Mode);
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,VRC2.PRG[i]);
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,x,data,VRC2.CHR[i].b0);
	SAVELOAD_BYTE(mode,x,data,VRC2.Mirror);
	x += MMC3_SaveLoad(mode,x,data);
	x += MMC1_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
	{
		SetMode();
		Sync();
	}
	return x;
}

static	void	MAPINT	Load (void)
{
	iNES_SetSRAM();
	MMC1_Load(Sync);
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	Mapper.WriteF = EMU->GetCPUWriteHandler(0xF);

	MMC1_Reset(ResetType);
	MMC3_Reset(ResetType);

	for (x = 0x4; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);

	if (ResetType == RESET_HARD)
	{
		VRC2.PRG[0] = 0;	VRC2.PRG[1] = 1;
		for (x = 0; x < 8; x++)
			VRC2.CHR[x].b0 = x;
		VRC2.Mirror = 0;
		Mapper.Mode = 0;
	}
	SetMode();
	Sync();
}

static	void	MAPINT	Unload (void)
{
	MMC1_Unload();
	MMC3_Unload();
}


static	u8 MapperNum = 116;
CTMapperInfo	MapperInfo_116 =
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