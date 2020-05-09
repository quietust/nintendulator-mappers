/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_latch.h"

namespace Latch
{
uint8_t Data;
uint16_n Addr;
FSync Sync;
BOOL BusConflicts;
BOOL SyncOnLoad;

void	Load (FSync _Sync, BOOL _BusConflicts, BOOL _SyncOnLoad)
{
	BusConflicts = _BusConflicts;
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
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
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, Addr.s0);
	SAVELOAD_BYTE(mode, offset, data, Data);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_AL (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Addr.b0);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_A (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, Addr.s0);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

int	MAPINT	SaveLoad_D (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Data);

	if (IsLoad(mode) && SyncOnLoad)
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
