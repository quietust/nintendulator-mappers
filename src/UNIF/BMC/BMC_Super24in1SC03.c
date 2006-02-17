#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 BankSize;
	u8 PRGbank, CHRbank;
}	Mapper;

static	void	Sync (void)
{
	int PRGmask[8] = {0x3F,0x1F,0x0F,0x01,0x03,0x00,0x00,0x00};
	MMC3_SyncMirror();
	MMC3_SyncWRAM();
	MMC3_SyncPRG(PRGmask[Mapper.BankSize & 0x7],Mapper.PRGbank << 1);
	if (Mapper.BankSize & 0x20)
		MMC3_SyncCHR_RAM(0x07,0);
	else	MMC3_SyncCHR_ROM(0xFF,Mapper.CHRbank << 3);
}

static	int	_MAPINT	SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.BankSize)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRGbank)
	SAVELOAD_BYTE(mode,x,data,Mapper.CHRbank)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr)
	{
	case 0xFF0:	Mapper.BankSize = Val;	break;
	case 0xFF1:	Mapper.PRGbank = Val;	break;
	case 0xFF2:	Mapper.CHRbank = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetCPUWriteHandler(0x5,Write);

	Mapper.BankSize = 0x24;
	Mapper.PRGbank = 0x9F;
	Mapper.CHRbank = 0;
	MMC3_Init(ResetType,Sync);
}

CTMapperInfo	MapperInfo_BMC_Super24in1SC03 =
{
	"BMC-Super24in1SC03",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};