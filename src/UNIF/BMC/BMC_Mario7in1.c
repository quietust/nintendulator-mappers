#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 WhichGame;
	u8 DidWrite;
	FCPUWrite Write67;
}	Mapper;

static	void	Sync (void)
{
	u8 PRGmask, CHRmask;
	u8 PRGbank, CHRbank;

	if (Mapper.WhichGame & 0x8)
	{
		PRGmask = 0x0F;
		PRGbank = Mapper.WhichGame & 0x07;
	}
	else
	{
		PRGmask = 0x1F;
		PRGbank = Mapper.WhichGame & 0x06;
	}
	if (Mapper.WhichGame & 0x40)
	{
		CHRmask = 0x7F;
		CHRbank = ((Mapper.WhichGame & 0x20) >> 3) | ((Mapper.WhichGame & 0x10) >> 4) | ((Mapper.WhichGame & 0x04) >> 1);
	}
	else
	{
		CHRmask = 0xFF;
		CHRbank = ((Mapper.WhichGame & 0x20) >> 3) | ((Mapper.WhichGame & 0x10) >> 4);
	}
	MMC3_SyncMirror();
	MMC3_SyncWRAM();
	MMC3_SyncPRG(PRGmask,PRGbank << 4);
	MMC3_SyncCHR_ROM(CHRmask,CHRbank << 7);
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichGame)
	SAVELOAD_BYTE(mode,x,data,Mapper.DidWrite)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write67 (int Bank, int Addr, int Val)
{
	Mapper.Write67(Bank,Addr,Val);
	if (Mapper.DidWrite)
		return;
	Mapper.WhichGame = Val;
	Mapper.DidWrite = 1;
	Sync();
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	Mapper.Write67 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUWriteHandler(0x6,Write67);
	EMU->SetCPUWriteHandler(0x7,Write67);

	if (ResetType == RESET_HARD)
	{
		Mapper.WhichGame = 0;
		Mapper.DidWrite = 0;
	}

	MMC3_Reset(ResetType);
}
static	void	_MAPINT	Unload (void)
{
	MMC3_Unload();
}

CTMapperInfo	MapperInfo_BMC_Mario7in1 =
{
	"BMC-Mario7in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};