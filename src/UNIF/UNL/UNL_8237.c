#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 PRG;
	u8 CHR;
	u8 Valid;
}	Mapper;

static	void	Sync (void)
{
//	MMC3_SyncCHR_ROM((Mapper.PRG & 0x40) ? 0x7F : 0xFF,(Mapper.CHR & 0x04) << 6);
	MMC3_SyncWRAM();
	if (Mapper.PRG & 0x40)
		MMC3_SyncCHR_ROM(0x7F,(Mapper.CHR & 0x04) << 5);
	else	MMC3_SyncCHR_ROM(0xFF,(Mapper.CHR & 0x04) << 6);
	MMC3_SyncMirror();
	if (Mapper.PRG & 0x80)
	{
		if (Mapper.PRG & 0x20)
			EMU->SetPRG_ROM32(0x8,(Mapper.PRG >> 1) & 0x7);
		else
		{
			EMU->SetPRG_ROM16(0x8,Mapper.PRG & 0xF);
			EMU->SetPRG_ROM16(0xC,Mapper.PRG & 0xF);
		}
	}
	else	MMC3_SyncPRG((Mapper.PRG & 0x40) ? 0x1F : 0x3F,0);
}

static	void	_MAPINT	Write5 (int Bank, int Addr, int Val)
{
	switch (Addr & 7)
	{
	case 0:	Mapper.PRG = Val;	break;
	case 1:	Mapper.CHR = Val;	break;
	}
	Sync();
}


static	void	_MAPINT	Write89 (int Bank, int Addr, int Val)
{
	MMC3_CPUWriteAB(Bank,0,Val | Val >> 7);
}

static	void	_MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,2,6,1,7,3,4,5};
	MMC3_CPUWrite89(Bank,0,(Val & 0xC0) | LUT[Val & 0x7]);
	Mapper.Valid = 1;
}

static	void	_MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Mapper.Valid)
		MMC3_CPUWrite89(Bank,1,Val);
	Mapper.Valid = 0;
}

static	void	_MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Val)
	{
		MMC3_CPUWriteCD(Bank,0,Val);
		MMC3_CPUWriteEF(Bank,1,Val);
	}
	else	MMC3_CPUWriteEF(Bank,0,Val);
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);
	EMU->SetCPUWriteHandler(0x5,Write5);
	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
	Mapper.PRG = Mapper.CHR = 0x00;
	Mapper.Valid = 1;
	Sync();
}
static	void	_MAPINT	Unload (void)
{
	MMC3_Unload();
}

CTMapperInfo	MapperInfo_UNL_8237 =
{
	"UNL-8237",
	"?",
	COMPAT_NEARLY,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};