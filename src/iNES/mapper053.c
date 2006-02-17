#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_Latch.h"

static	struct
{
	u8 Game;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM8(0x6,((Mapper.Game & 0xF) << 4) | 0xF);
	if (Mapper.Game & 0x10)
	{
		EMU->SetPRG_ROM16(0x8,((Mapper.Game & 0xF) << 3) | (Latch.Data & 0x7));
		EMU->SetPRG_ROM16(0xC,((Mapper.Game & 0xF) << 3) | 0x7);
	}
	else	EMU->SetPRG_ROM32(0x8,0x40); /* the EPROM at the end */
	EMU->SetCHR_RAM8(0,0);
	if (Mapper.Game & 0x20)
		EMU->Mirror_H();
	else	EMU->Mirror_V();
}

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	x = Latch_SaveLoad_D(mode,x,data);
	SAVELOAD_BYTE(mode,x,data,Mapper.Game)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Mapper.Game & 0x10)
		return;
	Mapper.Game = Val & 0x3F;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	Latch_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;
	for (x = 0x6; x < 0x8; x++)
		EMU->SetCPUWriteHandler(x,Write);
	Mapper.Game = 0;
	Latch_Init(ResetType,Sync,FALSE);
}

CTMapperInfo	MapperInfo_BMC_Supervision16in1 =
{
	"BMC-Supervision16in1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};