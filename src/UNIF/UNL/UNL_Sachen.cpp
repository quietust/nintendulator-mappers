/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

namespace
{
u8 Latch;
FCPUWrite _Write4;
FSync Sync;

void	Sync_72007 (void)
{
	EMU->SetPRG_ROM16(0x8, 0);
	EMU->SetPRG_ROM16(0xC, 1);
	EMU->SetCHR_ROM8(0, (Latch >> 7) & 0x1);
}
void	Sync_72008 (void)
{
	EMU->SetPRG_ROM32(0x8, (Latch >> 2) & 0x1);
	EMU->SetCHR_ROM8(0, Latch & 0x3);
}
void	Sync_0161M (void)
{
	EMU->SetPRG_ROM32(0x8, (Latch >> 3) & 0x1);
	EMU->SetCHR_ROM8(0, Latch & 0x7);
}
void	Sync_U0115M (void)
{
	EMU->SetPRG_ROM32(0x8, (Latch >> 2) & 0x1);
	EMU->SetCHR_ROM8(0, (Latch >> 3) & 0xF);
}
void	Sync_0036 (void)
{
	EMU->SetPRG_ROM32(0x8, 0);
	EMU->SetCHR_ROM8(0, (Latch::Data & 0x80) >> 7);
}
void	Sync_0037 (void)
{
	EMU->SetPRG_ROM32(0x8, (Latch::Data & 0x8) >> 3);
	EMU->SetCHR_ROM8(0, Latch::Data & 0x7);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Latch);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

void	MAPINT	WriteSA (int Bank, int Addr, int Val)
{
	if ((Bank == 4) && (Addr < 0x018))
		_Write4(Bank, Addr, Val);
	if (Addr & 0x100)
		Latch = Val;
	Sync();
}
void	MAPINT	WriteTC (int Bank, int Addr, int Val)
{
	if ((Bank == 4) && (Addr < 0x018))
		_Write4(Bank, Addr, Val);
	if ((Addr & 0x103) == 0x102)
		Latch = Val;
	Sync();
}

void	Reset (RESET_TYPE ResetType)
{
	Latch = 0;
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, WriteSA);
	EMU->SetCPUWriteHandler(0x5, WriteSA);
	UNIF_SetMirroring(NULL);
	Sync();
}

void	MAPINT	Reset_0161M (RESET_TYPE ResetType)
{
	Sync = Sync_0161M;
	Reset(ResetType);
}
void	MAPINT	Reset_72007 (RESET_TYPE ResetType)
{
	Sync = Sync_72007;
	Reset(ResetType);
}
void	MAPINT	Reset_72008 (RESET_TYPE ResetType)
{
	Sync = Sync_72008;
	Reset(ResetType);
}
void	MAPINT	Reset_U0115M (RESET_TYPE ResetType)
{
	Latch = 0;
	_Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4, WriteTC);
	EMU->SetCPUWriteHandler(0x5, WriteTC);
	EMU->SetCPUWriteHandler(0x6, WriteTC);
	EMU->SetCPUWriteHandler(0x7, WriteTC);
	EMU->SetCPUWriteHandler(0xC, WriteTC);
	EMU->SetCPUWriteHandler(0xD, WriteTC);
	EMU->SetCPUWriteHandler(0xE, WriteTC);
	EMU->SetCPUWriteHandler(0xF, WriteTC);
	UNIF_SetMirroring(NULL);
	Sync = Sync_U0115M;
	Sync();
}

void	MAPINT	Load_0036 (void)
{
	Latch::Load(Sync_0036, FALSE);
}
void	MAPINT	Load_0037 (void)
{
	Latch::Load(Sync_0037, FALSE);
}
void	MAPINT	Reset_003x (RESET_TYPE ResetType)
{
	Latch::Reset(ResetType);
	UNIF_SetMirroring(NULL);
}
void	MAPINT	Unload_003x (void)
{
	Latch::Unload();
}
} // namespace

CTMapperInfo	MapperInfo_UNL_SA_0161M =
{
	"UNL-SA-016-1M",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_0161M,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_72007 =
{
	"UNL-SA-72007",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_72007,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_72008 =
{
	"UNL-SA-72008",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_72008,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_TC_U0115M =
{
	"UNL-TC-U01-1.5M",
	_T("Sachen mapper"),
	COMPAT_FULL,
	NULL,
	Reset_U0115M,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_0036 =
{
	"UNL-SA-0036",
	_T("Sachen mapper"),
	COMPAT_FULL,
	Load_0036,
	Reset_003x,
	Unload_003x,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_0037 =
{
	"UNL-SA-0037",
	_T("Sachen mapper"),
	COMPAT_FULL,
	Load_0037,
	Reset_003x,
	Unload_003x,
	NULL,
	NULL,
	Latch::SaveLoad_D,
	NULL,
	NULL
};
