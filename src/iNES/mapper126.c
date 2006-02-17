#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	struct
{
	u8 Reg0, Reg1, Reg2, Reg3;
}	Mapper;

static	void	Sync (void)
{
	int PRGmask, CHRmask;
	int PRGbank, CHRbank;

	if (Mapper.Reg0 & 0x40)
	{
		PRGmask = 0x0F;
		PRGbank = (Mapper.Reg0 & 0x7) << 4;
	}
	else
	{
		PRGmask = 0x1F;
		PRGbank = (Mapper.Reg0 & 0x6) << 4;
	}
	PRGbank |= (Mapper.Reg0 & 0x10) << 3;

	if (Mapper.Reg0 & 0x80)
	{
		CHRmask = 0x7F;
		CHRbank = (Mapper.Reg0 & 0x08) << 4;
	}
	else
	{
		CHRmask = 0xFF;
		CHRbank = 0;
	}
	CHRbank |= (Mapper.Reg0 & 0x20) << 3;
	CHRbank |= (Mapper.Reg0 & 0x10) << 5;

	if (Mapper.Reg3 & 0x08)
		EMU->DbgOut("Alternate PRG switching used in mapper 126!");
	else
	{
		switch (Mapper.Reg3 & 0x3)
		{
		case 0:	MMC3_SyncPRG(PRGmask,PRGbank);	break;
		case 1:
		case 2:	EMU->SetPRG_ROM16(0x8,((MMC3_GetPRGBank(0) & PRGmask) | PRGbank) >> 1);
			EMU->SetPRG_ROM16(0xC,((MMC3_GetPRGBank(0) & PRGmask) | PRGbank) >> 1);	break;
		case 3:	EMU->SetPRG_ROM32(0x8,((MMC3_GetPRGBank(0) & PRGmask) | PRGbank) >> 2);	break;
		}
	}
	if (Mapper.Reg3 & 0x10)
	{
		if (!(Mapper.Reg0 & 0x80))
			CHRbank |= Mapper.Reg2 & 0x80;
		EMU->SetCHR_ROM8(0,(Mapper.Reg2 & 0xF) | (CHRbank >> 3));
	}
	else	MMC3_SyncCHR_ROM(CHRmask,CHRbank);
	MMC3_SyncMirror();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = MMC3_SaveLoad(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg0)
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg1)
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg2)
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg3)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 3)
	{
	case 0:	if (Mapper.Reg3 & 0x80)
			return;
		Mapper.Reg0 = Val;	break;
	case 1:	Mapper.Reg1 = Val;	break;
	case 2:	Mapper.Reg2 = Val;	break;
	case 3:	if (Mapper.Reg3 & 0x80)
			return;
		Mapper.Reg3 = Val;	break;
	}
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	Mapper.Reg0 = 0;
	Mapper.Reg1 = 0;
	Mapper.Reg2 = 0;
	Mapper.Reg3 = 0;

	MMC3_Init(ResetType,Sync);
}

static	u8 MapperNum = 126;
CTMapperInfo	MapperInfo_126 =
{
	&MapperNum,
	"Super Joy (MMC3)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};