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

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	MMC3_Destroy();
}

static	int	_MAPINT	Read (int Bank, int Where)
{
	unsigned char Data = Mapper.Read5(Bank,Where);
	if (Where & 0x800)
		return (Data & 0xFE) | (((~Where >> 8) | Where) & 1);
	else	return Data;
}

static	void	_MAPINT	Write5 (int Bank, int Where, int What)
{
	if (Where & 0x800)
		EMU->GetCPUWriteHandler(0x0)(Bank,Where,What);
}

static	void	_MAPINT	Write89 (int Bank, int Where, int What)
{
	if (Where & 1)
		MMC3_CPUWrite89(Bank,Where,What);
	else
	{
		unsigned char LUT[8] = {0,3,1,5,6,7,2,4};
		MMC3_CPUWrite89(Bank,Where,(What & 0xC0) | LUT[What & 0x7]);
	}
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	MMC3_Init(Sync);
	Mapper.Read5 = EMU->GetCPUReadHandler(0x5);
	EMU->SetCPUReadHandler(0x5,Read);
	EMU->SetCPUWriteHandler(0x5,Write5);
	for (x = 0x8; x < 0xA; x++)
		EMU->SetCPUWriteHandler(x,Write89);	/* need to override writes to $8000 */
}

CTMapperInfo	MapperInfo_UNL_H2288 =
{
	"UNL-H2288",
	"UNL-H2288",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	MMC3_SaveLoad,
	NULL,
	NULL
};