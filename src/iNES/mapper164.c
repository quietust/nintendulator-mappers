#include	"..\DLL\d_iNES.h"

static	struct
{
	FCPUWrite Write4;
	u8 Reg;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetCHR_RAM8(0,0);
	EMU->SetPRG_ROM32(0x8,Mapper.Reg);
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Reg)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	int Addr = (Bank << 12) | Where;
	if (Bank == 4) Mapper.Write4(Bank,Where,What);
	if ((Addr & 0x7300) == 0x5000)
		Mapper.Reg = What;
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

	Mapper.Write4 = EMU->GetCPUWriteHandler(0x4);
	EMU->SetPRG_RAM8(0x6,0);
	for (x = 4; x < 6; x++)
		EMU->SetCPUWriteHandler(x,Write);
	for (x = 8; x <= 0xF; x++)
		EMU->SetCPUWriteHandler(x,Write);

	Mapper.Reg = 0xFF;

	Sync();
}

static	u8 MapperNum = 164;
CTMapperInfo	MapperInfo_164 =
{
	&MapperNum,
	"Mapper 164",
	COMPAT_NEARLY,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};