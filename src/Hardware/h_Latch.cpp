/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"h_latch.h"

namespace Latch
{
uint8 Data;
uint16_n Addr;
FSync Sync;
BOOL BusConflicts;

void	Load (FSync _Sync, BOOL _BusConflicts)
{
	BusConflicts = _BusConflicts;
	Sync = _Sync;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		Data = 0;
		Addr.s0 = 0;
	}
	for (int i = 0x8; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);
	Sync();
}

void	Unload (void)
{
}

int	MAPINT	SaveLoad_AD (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_WORD(mode, offset, data, Addr.s0);
	SAVELOAD_BYTE(mode, offset, data, Data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_AL (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Addr.b0);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_A (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_WORD(mode, offset, data, Addr.s0);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_D (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_BYTE(mode, offset, data, Data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
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