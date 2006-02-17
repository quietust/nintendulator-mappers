#include	"..\..\DLL\d_UNIF.h"
#include	"..\..\Hardware\h_MMC3.h"

static	struct
{
	u8 Regs[4];
	u8 Pos;
}	Mapper;

static	void	Sync (void)
{
	MMC3_SyncMirror();
	MMC3_SyncPRG(~Mapper.Regs[3] & 0x3F,Mapper.Regs[1]);
	MMC3_SyncCHR_ROM(0xFF >> ((~Mapper.Regs[2]) & 0xF),(Mapper.Regs[0]) | ((Mapper.Regs[2] & 0xF0) << 4));
}

static	int	_MAPINT	SaveLoad (SAVELOAD_TYPE mode, int x, unsigned char *data)
{
	u8 i;
	x = MMC3_SaveLoad(mode,x,data);
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.Regs[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Pos);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	if (Mapper.Regs[3] & 0x40)
		return;
	Mapper.Regs[Mapper.Pos++] = Val;
	Mapper.Pos &= 0x03;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	MMC3_Destroy();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	u8 x;

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	for (x = 0; x < 4; x++)
		Mapper.Regs[x] = 0;
	Mapper.Pos = 0;

	MMC3_Init(ResetType,Sync);
}

CTMapperInfo	MapperInfo_BMC_Super1Min1 =
{
	"BMC-Super1Min1",
	"Pirate multicart mapper",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	MMC3_PPUCycle,
	SaveLoad,
	NULL,
	NULL
};