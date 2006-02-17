#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	struct
{
	u8 Latch;
	FCPUWrite Write4;
	void (*Sync)(void);
}	Mapper;

static	void	Sync_72007 (void)
{
	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,1);
	EMU->SetCHR_ROM8(0,(Mapper.Latch >> 7) & 0x1);
}
static	void	Sync_72008 (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch >> 2) & 0x1);
	EMU->SetCHR_ROM8(0,Mapper.Latch & 0x3);
}
static	void	Sync_0161M (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch >> 3) & 0x1);
	EMU->SetCHR_ROM8(0,Mapper.Latch & 0x7);
}
static	void	Sync_U0115M (void)
{
	EMU->SetPRG_ROM32(0x8,(Mapper.Latch >> 2) & 0x1);
	EMU->SetCHR_ROM8(0,(Mapper.Latch >> 3) & 0xF);
}
static	void	Sync_0036 (void)
{
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,(Latch.Data & 0x80) >> 7);
}
static	void	Sync_0037 (void)
{
	EMU->SetPRG_ROM32(0x8,(Latch.Data & 0x8) >> 3);
	EMU->SetCHR_ROM8(0,Latch.Data & 0x7);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Mapper.Sync();
	return x;
}

static	void	_MAPINT	WriteSA (int Bank, int Addr, int Val)
{
	if ((Bank == 4) && (Addr < 0x018))
		Mapper.Write4(Bank,Addr,Val);
	if (Addr & 0x100)
		Mapper.Latch = Val;
	Mapper.Sync();
}
static	void	_MAPINT	WriteTC (int Bank, int Addr, int Val)
{
	if ((Bank == 4) && (Addr < 0x018))
		Mapper.Write4(Bank,Addr,Val);
	if ((Addr & 0x103) == 0x102)
		Mapper.Latch = Val;
	Sync_U0115M();
}

static	void	Reset (RESET_TYPE ResetType)
{
	Mapper.Latch = 0;
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,WriteSA);
	EMU->SetCPUWriteHandler(0x5,WriteSA);
	UNIF_SetMirroring(NULL);
	Mapper.Sync();
}

static	void	_MAPINT	Reset_0161M (RESET_TYPE ResetType)
{
	Mapper.Sync = Sync_0161M;
	Reset(ResetType);
}
static	void	_MAPINT	Reset_72007 (RESET_TYPE ResetType)
{
	Mapper.Sync = Sync_72007;
	Reset(ResetType);
}
static	void	_MAPINT	Reset_72008 (RESET_TYPE ResetType)
{
	Mapper.Sync = Sync_72008;
	Reset(ResetType);
}
static	void	_MAPINT	Reset_U0115M (RESET_TYPE ResetType)
{
	Mapper.Latch = 0;
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,WriteTC);
	EMU->SetCPUWriteHandler(0x5,WriteTC);
	EMU->SetCPUWriteHandler(0x6,WriteTC);
	EMU->SetCPUWriteHandler(0x7,WriteTC);
	EMU->SetCPUWriteHandler(0xC,WriteTC);
	EMU->SetCPUWriteHandler(0xD,WriteTC);
	EMU->SetCPUWriteHandler(0xE,WriteTC);
	EMU->SetCPUWriteHandler(0xF,WriteTC);
	UNIF_SetMirroring(NULL);
	Sync_U0115M();
}

static	void	_MAPINT	Load_0036 (void)
{
	Latch_Load(Sync_0036,FALSE);
}
static	void	_MAPINT	Load_0037 (void)
{
	Latch_Load(Sync_0037,FALSE);
}
static	void	_MAPINT	Reset_003x (RESET_TYPE ResetType)
{
	Latch_Reset(ResetType);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Unload_003x (void)
{
	Latch_Unload();
}

CTMapperInfo	MapperInfo_UNL_SA_0161M =
{
	"UNL-SA-016-1M",
	"Sachen mapper",
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
	"Sachen mapper",
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
	"Sachen mapper",
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
	"Sachen mapper",
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
	"Sachen mapper",
	COMPAT_FULL,
	Load_0036,
	Reset_003x,
	Unload_003x,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_0037 =
{
	"UNL-SA-0037",
	"Sachen mapper",
	COMPAT_FULL,
	Load_0037,
	Reset_003x,
	Unload_003x,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
