/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"

namespace N108
{
uint8 Cmd;
uint8 PRG[2];
uint8 CHR[6];
FSync Sync;

void	MAPINT	CPUWrite89 (int Bank, int Addr, int Val);

void	Load (FSync _Sync)
{
	Sync = _Sync;
}
void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0x3C;	PRG[1] = 0x3D;

		CHR[0] = 0x00;	CHR[1] = 0x02;
		CHR[2] = 0x04;	CHR[3] = 0x05;	CHR[4] = 0x06;	CHR[5] = 0x07;
		Cmd = 0;
	}
	EMU->SetCPUWriteHandler(0x8, CPUWrite89);
	EMU->SetCPUWriteHandler(0x9, CPUWrite89);
	Sync();
}
void	Unload (void)
{
	Sync = NULL;
}

void	SyncPRG (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, 0x3F);
}

void	SyncCHR (void)
{
	EMU->SetCHR_ROM2(0, CHR[0]);
	EMU->SetCHR_ROM2(2, CHR[1]);
	EMU->SetCHR_ROM1(4, CHR[2]);
	EMU->SetCHR_ROM1(5, CHR[3]);
	EMU->SetCHR_ROM1(6, CHR[4]);
	EMU->SetCHR_ROM1(7, CHR[5]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUWrite89 (int Bank, int Addr, int Val)
{
	Val &= 0x3F;
	if (Addr & 1)
		switch (Cmd & 0x7)
		{
		case 0:	CHR[0] = Val >> 1;	break;
		case 1:	CHR[1] = Val >> 1;	break;
		case 2:	CHR[2] = Val;		break;
		case 3:	CHR[3] = Val;		break;
		case 4:	CHR[4] = Val;		break;
		case 5:	CHR[5] = Val;		break;
		case 6:	PRG[0] = Val;		break;
		case 7:	PRG[1] = Val;		break;
		}
	else	Cmd = Val;
	Sync();
}
} // namespace N108

namespace
{
void	Sync_DEROM (void)
{
	N108::SyncPRG();
	N108::SyncCHR();
	UNIF_SetMirroring(NULL);
}
void	Sync_DEIROM (void)
{
	N108::SyncPRG();
	N108::SyncCHR();
	UNIF_SetMirroring(NULL);
}
void	Sync_DRROM (void)
{
	N108::SyncPRG();
	N108::SyncCHR();
	EMU->Mirror_4();
}

void	MAPINT	Load_DEROM (void)
{
	N108::Load(Sync_DEROM);
}
void	MAPINT	Load_DEIROM (void)
{
	N108::Load(Sync_DEIROM);
}
void	MAPINT	Load_DRROM (void)
{
	N108::Load(Sync_DRROM);
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	N108::Reset(ResetType);
}
void	MAPINT	Unload (void)
{
	N108::Unload();
}
} // namespace

const MapperInfo MapperInfo_NES_DEROM =
{
	"NES-DEROM",
	_T("Namco 108"),
	COMPAT_NEARLY,
	Load_DEROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N108::SaveLoad,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_DEIROM =
{
	"NES-DEIROM",
	_T("Namco 108"),
	COMPAT_NEARLY,
	Load_DEIROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N108::SaveLoad,
	NULL,
	NULL
};
const MapperInfo MapperInfo_NES_DRROM =
{
	"NES-DRROM",
	_T("Namco 108 with 4-screen VRAM"),
	COMPAT_NEARLY,
	Load_DRROM,
	Reset,
	Unload,
	NULL,
	NULL,
	N108::SaveLoad,
	NULL,
	NULL
};