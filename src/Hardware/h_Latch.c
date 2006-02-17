#include	"h_latch.h"

TLatch	Latch;

void	Latch_Init (RESET_TYPE ResetType, void (*Sync)(void), BOOL BusConflicts)
{
	u8 x;
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Latch_Write);
	if (ResetType == RESET_HARD)
	{
		Latch.Data = 0;
		Latch.Addr.s0 = 0;
	}
	Latch.BusConflicts = BusConflicts;
	(Latch.Sync = Sync)();
}

void	Latch_Destroy (void)
{
}

int	_MAPINT	Latch_SaveLoad_AD (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_WORD(mode,x,data,Latch.Addr.s0)
	SAVELOAD_BYTE(mode,x,data,Latch.Data)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_AL (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Latch.Addr.b0)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_A (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_WORD(mode,x,data,Latch.Addr.s0)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_D (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Latch.Data)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

#include <stdlib.h>
void	_MAPINT	Latch_Write (int Bank, int Addr, int Val)
{
#ifdef BUS_CONFLICTS
	int tmp;
	if ((Latch.BusConflicts) && ((tmp = EMU->GetCPUReadHandler(Bank)(Bank,Addr)) != Val))
	{
		if (rand() & 1)
		{
			Val = tmp;
			EMU->StatusOut("Bus conflict - using ROM data");
		}
		else	EMU->StatusOut("Bus conflict - using CPU data");
	}
#endif
	Latch.Data = Val;
	Latch.Addr.s0 = Addr;
	Latch.Addr.n3 = Bank;
	Latch.Sync();
}