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
	MMC3_SyncPRG(PRGmask,PRGbank << 4);
	MMC3_SyncCHR_ROM(CHRmask,CHRbank << 7);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.WhichGame)
	SAVELOAD_BYTE(mode,x,data,Mapper.DidWrite)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write67 (int Bank, int Where, int What)
{
	Mapper.Write67(Bank,Where,What);
	if (Mapper.DidWrite)
		return;
	Mapper.WhichGame = What;
	Mapper.DidWrite = 1;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	Mapper.Write67 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUWriteHandler(0x6,Write67);
	EMU->SetCPUWriteHandler(0x7,Write67);

	EMU->SetPRG_RAM8(0x6,0);		/* WRAM goes here */

	Mapper.WhichGame = 0;
	Mapper.DidWrite = 0;

	MMC3_Init(Sync);
}

CTMapperInfo	MapperInfo_BMC_Mario7in1 =
{
	-1,
	"BMC-Mario7in1",
	"BMC-Mario7in1",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};