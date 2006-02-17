#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	FCPURead	Read5;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncWRAM();
	MMC3_SyncPRG(0x3F,0);
	MMC3_SyncCHR_ROM(0xFF,0);
	MMC3_SyncMirror();
}

static	int	_MAPINT	Read (int Bank, int Addr)
{
	unsigned char Data = Mapper.Read5(Bank,Addr);
	if (Addr & 0x800)
		return (Data & 0xFE) | (((~Addr >> 8) | Addr) & 1);
	else	return Data;
}

static	void	_MAPINT	Write5 (int Bank, int Addr, int Val)
{
	if (Addr & 0x800)
		EMU->GetCPUWriteHandler(0x0)(Bank,Addr,Val);
}

static	void	_MAPINT	Write89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		MMC3_CPUWrite89(Bank,Addr,Val);
	else
	{
		unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3_CPUWrite89(Bank,Addr,(Val & 0xC0) | LUT[Val & 0x7]);
	}
}

static	void	_MAPINT	Load (void)
{
	MMC3_Load(Sync);
}
static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	MMC3_Reset(ResetType);
	Mapper.Read5 = EMU->GetCPUReadHandler(0x5);
	EMU->SetCPUReadHandler(0x5,Read);
	EMU->SetCPUWriteHandler(0x5,Write5);
	for (x = 0x8; x < 0xA; x++)
		EMU->SetCPUWriteHandler(x,Write89);	/* need to override writes to $8000 */
}
static	void	_MAPINT	Unload (void)
{
	MMC3_Unload();
}


CTMapperInfo	MapperInfo_UNL_H2288 =
{
	"UNL-H2288",
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