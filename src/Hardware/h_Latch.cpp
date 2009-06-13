/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_latch.h"

namespace Latch
{
u8 Data;
u16_n Addr;
FSync Sync;
BOOL BusConflicts;

void	Load (FSync _Sync, BOOL _BusConflicts)
{
	BusConflicts = _BusConflicts;
	Sync = _Sync;
}

void	Reset (RESET_TYPE ResetType)
{
	u8 x;
	if (ResetType == RESET_HARD)
	{
		Data = 0;
		Addr.s0 = 0;
	}
	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x, Write);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad_AD (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_WORD(mode, x, data, Addr.s0);
	SAVELOAD_BYTE(mode, x, data, Data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

int	MAPINT	SaveLoad_AL (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Addr.b0);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

int	MAPINT	SaveLoad_A (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_WORD(mode, x, data, Addr.s0);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

int	MAPINT	SaveLoad_D (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode, x, data, Data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

#include <stdlib.h>
void	MAPINT	Write (int Bank, int _Addr, int Val)
{
#ifdef BUS_CONFLICTS
	int tmp;
	if ((BusConflicts) && ((tmp = EMU->GetCPUReadHandler(Bank)(Bank, _Addr)) != Val))
	{
		if (rand() & 1)
		{
			Val = tmp;
			EMU->StatusOut(_T("Bus conflict - using ROM data"));
		}
		else	EMU->StatusOut(_T("Bus conflict - using CPU data"));
	}
#endif
	Data = Val;
	Addr.s0 = _Addr;
	Addr.n3 = Bank;
	Sync();
}
} // namespace Latch