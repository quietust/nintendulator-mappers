#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	void	Sync (void)
{
	u8 What = EMU->GetCPUReadHandler(Latch.Addr.s0 >> 12)(Latch.Addr.n3,Latch.Addr.s0 & 0xFFF);
	if (What & 0x0E)
	{
		EMU->SetPRG_ROM16(0x8,0x0F);
		EMU->SetPRG_ROM16(0xC,What & 0x0F);
	}
	else	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,What & 0x0F);
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	Latch_Init(Sync,IsHardReset,FALSE);
	UNIF_SetMirroring(NULL);
}

CTMapperInfo	MapperInfo_BMC_Generic15in1 =
{
	"BMC-Generic15in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	Latch_SaveLoad_A,
	NULL,
	NULL
};