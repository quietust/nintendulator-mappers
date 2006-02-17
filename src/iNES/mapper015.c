#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 PRG[4];
	u8 Mirror;
}	Mapper;

static	void	Sync (void)
{
	u8 x;
	for (x = 0; x < 4; x++)
		EMU->SetPRG_ROM8(0x8 | (x << 1),Mapper.PRG[x]);
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Mirror)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.PRG[i]);
	SAVELOAD_BYTE(mode,x,data,Mapper.Mirror);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	u8 PRGbank = (Val & 0x3F) << 1;
	switch (Addr)
	{
	case 0x000:
		if (Val & 0x80)
		{
			Mapper.PRG[0] = PRGbank + 1;
			Mapper.PRG[1] = PRGbank + 0;
			Mapper.PRG[2] = PRGbank + 3;
			Mapper.PRG[3] = PRGbank + 2;
		}
		else
		{
			Mapper.PRG[0] = PRGbank + 0;
			Mapper.PRG[1] = PRGbank + 1;
			Mapper.PRG[2] = PRGbank + 2;
			Mapper.PRG[3] = PRGbank + 3;
		}
		Mapper.Mirror = Val & 0x40;
		break;
	case 0x001:
		if (Val & 0x80)
		{
			Mapper.PRG[0] = PRGbank + 1;
			Mapper.PRG[1] = PRGbank + 0;
		}
		else
		{
			Mapper.PRG[0] = PRGbank + 0;
			Mapper.PRG[1] = PRGbank + 1;
		}
		break;
	case 0x002:
		if (Val & 0x80)
		{
			Mapper.PRG[0] = PRGbank + 1;
			Mapper.PRG[1] = PRGbank + 1;
			Mapper.PRG[2] = PRGbank + 1;
			Mapper.PRG[3] = PRGbank + 1;
		}
		else
		{
			Mapper.PRG[0] = PRGbank + 0;
			Mapper.PRG[1] = PRGbank + 0;
			Mapper.PRG[2] = PRGbank + 0;
			Mapper.PRG[3] = PRGbank + 0;
		}
		break;
	case 0x003:
		if (Val & 0x80)
		{
			Mapper.PRG[2] = PRGbank + 1;
			Mapper.PRG[3] = PRGbank + 0;
		}
		else
		{
			Mapper.PRG[2] = PRGbank + 0;
			Mapper.PRG[3] = PRGbank + 1;
		}
		Mapper.Mirror = Val & 0x40;
		break;
	}
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x8,Write);

	for (x = 0; x < 4; x++)
		Mapper.PRG[x] = x;
	Mapper.Mirror = 0;
	Sync();
}

static	u8 MapperNum = 15;
CTMapperInfo	MapperInfo_015 =
{
	&MapperNum,
	"100-in-1 Contra Function 16",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};