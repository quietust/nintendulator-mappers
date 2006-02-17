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

static	int	_MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.Latch)
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Latch = Val;
	Sync();
}

static	void	_MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_InitROM();

	EMU->SetCPUWriteHandler(0x6,Write);
	EMU->SetCPUWriteHandler(0x7,Write);

	Mapper.Latch = 0;

	Sync();
}

static	u8 MapperNum = 184;
CTMapperInfo	MapperInfo_184 =
{
	&MapperNum,
	"Mapper 184 (Atlantis no Nazo)",
	COMPAT_FULL,
	Reset,
	NULL,
	NULL,
	NULL,
	SaveLoad,
	NULL,
	NULL
};