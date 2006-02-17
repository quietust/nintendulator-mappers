#include	"..\..\DLL\d_UNIF.h"

typedef	struct	N108
{
	u8 Cmd;
	u8 PRG[2];
	u8 CHR[6];
	void	(*Sync)	(void);
}	TN108, *PN108;
static	TN108	N108;

void	_MAPINT	N108_CPUWrite89 (int Bank, int Addr, int Val);

void	N108_Init (RESET_TYPE ResetType, void (*Sync)(void))
{
	N108.PRG[0] = 0x3C;	N108.PRG[1] = 0x3D;

	N108.CHR[0] = 0x00;	N108.CHR[1] = 0x02;
	N108.CHR[2] = 0x04;	N108.CHR[3] = 0x05;	N108.CHR[4] = 0x06;	N108.CHR[5] = 0x07;

	N108.Cmd = 0;
	EMU->SetCPUWriteHandler(0x8,N108_CPUWrite89);
	EMU->SetCPUWriteHandler(0x9,N108_CPUWrite89);
	(N108.Sync = Sync)();
}

void	N108_Destroy (void)
{
	N108.Sync = NULL;
}

void	N108_SyncPRG (void)
{
	EMU->SetPRG_ROM8(0x8,N108.PRG[0]);
	EMU->SetPRG_ROM8(0xA,N108.PRG[1]);
	EMU->SetPRG_ROM16(0xC,0x3F);
}

void	N108_SyncCHR (void)
{
	EMU->SetCHR_ROM2(0,N108.CHR[0]);
	EMU->SetCHR_ROM2(2,N108.CHR[1]);
	EMU->SetCHR_ROM1(4,N108.CHR[2]);
	EMU->SetCHR_ROM1(5,N108.CHR[3]);
	EMU->SetCHR_ROM1(6,N108.CHR[4]);
	EMU->SetCHR_ROM1(7,N108.CHR[5]);
}

int	_MAPINT	N108_SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	SAVELOAD_BYTE(mode,x,data,N108.Cmd)
	for (i = 0; i < 2; i++)
		SAVELOAD_BYTE(mode,x,data,N108.PRG[i])
	for (i = 0; i < 6; i++)
		SAVELOAD_BYTE(mode,x,data,N108.CHR[i])
	if (mode == STATE_LOAD)
		N108.Sync();
	return x;
}

void	_MAPINT	N108_CPUWrite89 (int Bank, int Addr, int Val)
{
	Val &= 0x3F;
	if (Addr & 1)
		switch (N108.Cmd & 0x7)
		{
		case 0:	N108.CHR[0] = Val >> 1;break;
		case 1:	N108.CHR[1] = Val >> 1;break;
		case 2:	N108.CHR[2] = Val;	break;
		case 3:	N108.CHR[3] = Val;	break;
		case 4:	N108.CHR[4] = Val;	break;
		case 5:	N108.CHR[5] = Val;	break;
		case 6:	N108.PRG[0] = Val;	break;
		case 7:	N108.PRG[1] = Val;	break;
		}
	else	N108.Cmd = Val;
	N108.Sync();
}

static	void	Sync_DEROM (void)
{
	N108_SyncPRG();
	N108_SyncCHR();
	UNIF_SetMirroring(NULL);
}
static	void	Sync_DEIROM (void)
{
	N108_SyncPRG();
	N108_SyncCHR();
	UNIF_SetMirroring(NULL);
}
static	void	Sync_DRROM (void)
{
	N108_SyncPRG();
	N108_SyncCHR();
	EMU->Mirror_4();
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
	N108_Destroy();
}

static	void	_MAPINT	Reset_DEROM (RESET_TYPE ResetType)
{
	N108_Init(ResetType,Sync_DEROM);
}
static	void	_MAPINT	Reset_DEIROM (RESET_TYPE ResetType)
{
	N108_Init(ResetType,Sync_DEIROM);
}
static	void	_MAPINT	Reset_DRROM (RESET_TYPE ResetType)
{
	N108_Init(ResetType,Sync_DRROM);
}

CTMapperInfo	MapperInfo_NES_DEROM =
{
	"NES-DEROM",
	"Namco 108",
	COMPAT_NEARLY,
	Reset_DEROM,
	Shutdown,
	NULL,
	NULL,
	N108_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_DEIROM =
{
	"NES-DEIROM",
	"Namco 108",
	COMPAT_NEARLY,
	Reset_DEIROM,
	Shutdown,
	NULL,
	NULL,
	N108_SaveLoad,
	NULL,
	NULL
};
CTMapperInfo	MapperInfo_NES_DRROM =
{
	"NES-DRROM",
	"Namco 108 with 4-screen VRAM",
	COMPAT_NEARLY,
	Reset_DRROM,
	Shutdown,
	NULL,
	NULL,
	N108_SaveLoad,
	NULL,
	NULL
};

