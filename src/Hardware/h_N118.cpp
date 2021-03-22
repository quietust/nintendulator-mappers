/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"h_N118.h"

namespace N118
{
uint8_t Cmd;
uint8_t PRG[2];
uint8_t CHR[6];
FSync Sync;
BOOL SyncOnLoad;

void	Load (FSync _Sync, BOOL _SyncOnLoad)
{
	Sync = _Sync;
	SyncOnLoad = _SyncOnLoad;
}
void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		PRG[0] = 0xC;	PRG[1] = 0xD;

		CHR[0] = 0x00;	CHR[1] = 0x02;
		CHR[2] = 0x04;	CHR[3] = 0x05;	CHR[4] = 0x06;	CHR[5] = 0x07;
		Cmd = 0;
	}
	EMU->SetCPUWriteHandler(0x8, CPUWrite89);
	EMU->SetCPUWriteHandler(0x9, CPUWrite89);
	Sync();
}
void	Unload (void)
{
	Sync = NULL;
}

void	SyncPRG (void)
{
	EMU->SetPRG_ROM8(0x8, PRG[0]);
	EMU->SetPRG_ROM8(0xA, PRG[1]);
	EMU->SetPRG_ROM16(0xC, 0xF);
}

void	SyncCHR (void)
{
	EMU->SetCHR_ROM2(0x0, CHR[0] >> 1);
	EMU->SetCHR_ROM2(0x2, CHR[1] >> 1);
	EMU->SetCHR_ROM1(0x4, CHR[2]);
	EMU->SetCHR_ROM1(0x5, CHR[3]);
	EMU->SetCHR_ROM1(0x6, CHR[4]);
	EMU->SetCHR_ROM1(0x7, CHR[5]);
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

// Alternate version, originally used by "N108" module in NES_DxROM
// It stored pre-shifted values for CHR banks 0 and 1 and was thus
// incompatible with the above code.
int	MAPINT	SaveLoad_Alt (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Cmd);
	for (int i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode, offset, data, PRG[i]);
	for (int i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	if (IsLoad(mode) && (ver == 0))
	{
		CHR[0] <<= 1;
		CHR[1] <<= 1;
	}

	if (IsLoad(mode) && SyncOnLoad)
		Sync();
	return offset;
}

void	MAPINT	CPUWrite89 (int Bank, int Addr, int Val)
{
	if (Addr & 1)
		switch (Cmd & 0x7)
		{
		case 0:	CHR[0] = Val & 0x3E;	break;
		case 1:	CHR[1] = Val & 0x3E;	break;
		case 2:	CHR[2] = Val & 0x3F;	break;
		case 3:	CHR[3] = Val & 0x3F;	break;
		case 4:	CHR[4] = Val & 0x3F;	break;
		case 5:	CHR[5] = Val & 0x3F;	break;
		case 6:	PRG[0] = Val & 0xF;	break;
		case 7:	PRG[1] = Val & 0xF;	break;
		}
	else	Cmd = Val;	// not masking it here, since one mapper uses some upper bits
	Sync();
}
} // namespace N118
