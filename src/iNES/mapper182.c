#include	"..\DLL\d_iNES.h"
#include	"..\Hardware\h_MMC3.h"

static	void	Sync (void)
{
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncWRAM();
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
}

static	void	MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3_CPUWriteAB(Bank,0,Val);
}

static	void	MAPINT	WriteAB (int Bank, int Addr, int Val)
{
	unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
	if (Addr & 1)
		;
	else	MMC3_CPUWrite89(Bank,0,LUT[Val & 0x7] | (Val & 0xC0));
}

static	void	MAPINT	WriteCD (int Bank, int Addr, int Val)
{
	if (Addr & 1)
	{
		MMC3_CPUWriteCD(Bank,0,Val);
		MMC3_CPUWriteCD(Bank,1,Val);
	}
	else	MMC3_CPUWrite89(Bank,1,Val);
}

static	void	MAPINT	WriteEF (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3_CPUWriteEF(Bank,1,Val);
	else	MMC3_CPUWriteEF(Bank,0,Val);
}

static	void	MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	MMC3_Reset(ResetType);

	EMU->SetCPUWriteHandler(0x8,Write89);
	EMU->SetCPUWriteHandler(0x9,Write89);
	EMU->SetCPUWriteHandler(0xA,WriteAB);
	EMU->SetCPUWriteHandler(0xB,WriteAB);
	EMU->SetCPUWriteHandler(0xC,WriteCD);
	EMU->SetCPUWriteHandler(0xD,WriteCD);
	EMU->SetCPUWriteHandler(0xE,WriteEF);
	EMU->SetCPUWriteHandler(0xF,WriteEF);
}
static	void	MAPINT	Unload (void)
{
	MMC3_Unload();
}

static	u8 MapperNum = 182;
CTMapperInfo	MapperInfo_182 =
{
	&MapperNum,
	_T("Super Donkey Kong"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};