#include	"h_latch.h"

TLatch	Latch;

void	Latch_Init (void (*Sync)(void), BOOL Reset, BOOL BusConflicts)
{
	u8 x;
	for (x = 0x8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Latch_Write);
	if (Reset)
	{
		Latch.Data = 0;
		Latch.Addr = 0;
	}
	Latch.BusConflicts = BusConflicts;
	(Latch.Sync = Sync)();
}

void	Latch_Destroy (void)
{
}

int	_MAPINT	Latch_SaveLoad_AD (int mode, int x, char *data)
{
	SAVELOAD_WORD(mode,x,data,Latch.Addr)
	SAVELOAD_BYTE(mode,x,data,Latch.Data)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_AL (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Latch.AddrL)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_A (int mode, int x, char *data)
{
	SAVELOAD_WORD(mode,x,data,Latch.Addr)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

int	_MAPINT	Latch_SaveLoad_D (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Latch.Data)
	if (mode == STATE_LOAD)
		Latch.Sync();
	return x;
}

void	_MAPINT	Latch_Write (int Bank, int Where, int What)
{
#ifdef BUS_CONFLICTS
	if ((Latch.BusConflicts) && (EMU->GetReadHandler(Bank)(Bank,Where) != What))
	{
		EMU->DbgOut("Bus conflict!");
		return;
	}
#endif
	Latch.Data = What;
	Latch.Addr = (Bank << 12) | Where;
	Latch.Sync();
}