#include	"..\DLL\d_iNES.h"

static	struct
{
	u8 Latch;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,-1);
	EMU->SetCHR_ROM4(0,((Mapper.Latch & 0x02) << 0) | ((Mapper.Latch & 0x04) >> 2) | ((Mapper.Latch & 0x01) << 2));
	EMU->SetCHR_ROM4(4,((Mapper.Latch & 0x20) >> 4) | ((Mapper.Latch & 0x10) >> 1));

//	BANK_CHR4(0x0000, (value & 0x02) + ((value & 0x04)>>2)); /* use bank 1, 3 */
//	BANK_CHR4(0x1000, (value & 0x20) >> 4); /* use bank 0, 2 */

}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	Mapper.Latch = What;
	Sync();
}

static	void	_MAPINT	Shutdown (void)
{
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	Mapper.Latch = 0;

	Sync();
}

CTMapperInfo	MapperInfo_184 =
{
	184,
	NULL,
	"Mapper 184 (Atlantis no Nazo)",
	COMPAT_FULL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
