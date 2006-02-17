#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_Latch.h"

static	struct
{
	u8 Pos;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM32(0x8,Latch.Data & 0x3);
	EMU->SetCHR_RAM4(0,(Latch.Data & 0x4) | Mapper.Pos);
	EMU->SetCHR_RAM4(4,(Latch.Data & 0x4) | 3);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch_SaveLoad_D(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Pos)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	u8 newpos;
	if ((Addr & 0x3000) != 0x2000)	return;
	if ((Addr & 0x3FF) >= 0x3C0)	return;
	newpos = (Addr >> 8) & 3;
	if (Mapper.Pos != newpos)
	{
		Mapper.Pos = newpos;
		Sync();
	}
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();
	Latch_Init(ResetType,Sync,FALSE);
	Mapper.Pos = 0;
}

static	u8 MapperNum = 96;
CTMapperInfo	MapperInfo_096 =
{
	&MapperNum,
	"Bandai 74161/7432",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	PPUCycle,
	SaveLoad,
	NULL,
	NULL
};