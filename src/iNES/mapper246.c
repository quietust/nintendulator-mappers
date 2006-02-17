#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPURead Read6;
	FCPUWrite Write6;
	u8 PRG[4], CHR[4];
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
	{
		EMU->SetPRG_ROM8(0x8 | (x << 1),Mapper.PRG[x]);
		EMU->SetCHR_ROM2(x << 1,Mapper.CHR[x]);
	}
	EMU->SetPRG_RAM4(0x6,0);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i])
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	int	_MAPINT	Read6 (int Bank, int Where)
{
	if (Where & 0x800)
		return Mapper.Read6(Bank,Where & 0x7FF);
	else	return -1;
}

static	void	_MAPINT	Write6 (int Bank, int Where, int What)
{
	if (Where & 0x800)
	{
		Mapper.Write6(Bank,Where & 0x7FF,What);
		return;
	}
	switch (Where & 0x007)
	{
	case 0:	Mapper.PRG[0] = What;	break;
	case 1:	Mapper.PRG[1] = What;	break;
	case 2:	Mapper.PRG[2] = What;	break;
	case 3:	Mapper.PRG[3] = What;	break;
	case 4:	Mapper.CHR[0] = What;	break;
	case 5:	Mapper.CHR[1] = What;	break;
	case 6:	Mapper.CHR[2] = What;	break;
	case 7:	Mapper.CHR[3] = What;	break;
	}
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;

	iNES_InitROM();

	Mapper.Read6 = EMU->GetCPUReadHandler(0x6);
	Mapper.Write6 = EMU->GetCPUWriteHandler(0x6);
	EMU->SetCPUReadHandler(0x6,Read6);
	EMU->SetCPUWriteHandler(0x6,Write6);

	Mapper.PRG[0] = 0;	Mapper.PRG[1] = 1;
	Mapper.PRG[2] = 0xFE;	Mapper.PRG[3] = 0xFF;
	for (x = 0; x < 4; x++)
		Mapper.CHR[x] = 0;

	Sync();
}

static	u8 MapperNum = 246;
CTMapperInfo	MapperInfo_246 =
{
	&MapperNum,
	"Mapper 246",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};