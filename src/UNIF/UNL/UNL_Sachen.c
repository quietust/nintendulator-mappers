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

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Mapper.Sync();
	return x;
}

static	void	_MAPINT	WriteSA (int Bank, int Where, int What)
{
	if ((Bank == 4) && (Where < 0x018))
		Mapper.Write4(Bank,Where,What);
	if (Where & 0x100)
		Mapper.Latch = What;
	Mapper.Sync();
}
static	void	_MAPINT	WriteTC (int Bank, int Where, int What)
{
	if ((Bank == 4) && (Where < 0x018))
		Mapper.Write4(Bank,Where,What);
	if ((Where & 0x103) == 0x102)
		Mapper.Latch = What;
	Sync_U0115M();
}

static	void	Reset (int IsHardReset)
{
	Mapper.Latch = 0;
	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,WriteSA);
	EMU->SetCPUWriteHandler(0x5,WriteSA);
	UNIF_SetMirroring(NULL);
	Mapper.Sync();
}

static	void	_MAPINT	Reset_0161M (int IsHardReset)
{
	Mapper.Sync = Sync_0161M;
	Reset(IsHardReset);
}
static	void	_MAPINT	Reset_72007 (int IsHardReset)
{
	Mapper.Sync = Sync_72007;
	Reset(IsHardReset);
}
static	void	_MAPINT	Reset_72008 (int IsHardReset)
{
	Mapper.Sync = Sync_72008;
	Reset(IsHardReset);
}
static	void	_MAPINT	Reset_U0115M (int IsHardReset)
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
static	void	_MAPINT	Reset_0036 (int IsHardReset)
{
	Latch_Init(Sync_0036,IsHardReset,FALSE);
	UNIF_SetMirroring(NULL);
}
static	void	_MAPINT	Reset_0037 (int IsHardReset)
{
	Latch_Init(Sync_0037,IsHardReset,FALSE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_UNL_SA_0161M =
{
	"UNL-SA-016-1M",
	"UNL-SA-016-1M",
	COMPAT_FULL,
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
	"UNL-SA-72007",
	COMPAT_FULL,
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
	"UNL-SA-72008",
	COMPAT_FULL,
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
	"UNL-TC-U01-1.5M",
	COMPAT_FULL,
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
	"UNL-SA-0036",
	COMPAT_FULL,
	Reset_0036,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_UNL_SA_0037 =
{
	"UNL-SA-0037",
	"UNL-SA-0037",
	COMPAT_FULL,
	Reset_0037,
	NULL,
	NULL,
	NULL,
	Latch_SaveLoad_D,
	NULL,
	NULL
};
