#include	"..\DLL\d_VS.h"
#include	"..\Hardware\h_VS.h"

static	struct
{
	FCPUWrite Write;
	u8 CHR;
}	Mapper;

static	void	Sync (void)
{
	EMU->SetPRG_RAM8(0x6,0);
	EMU->SetPRG_ROM32(0x8,0);
	EMU->SetCHR_ROM8(0,Mapper.CHR);
}

static	int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	SAVELOAD_BYTE(mode,x,data,Mapper.CHR)
	x = VS_SaveLoad(mode,x,data);
	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	MAPINT	Write (int Bank, int Addr, int Val)
{
	Mapper.Write(Bank,Addr,Val);
	if (Addr == 0x016)
	{
		Mapper.CHR = (Val & 0x04) >> 2;
		Sync();
	}
}

static	void	MAPINT	Load (void)
{
	VS_Load();
}

static	void	MAPINT	Reset (RESET_TYPE ResetType)
{
	iNES_SetMirroring();

	VS_Reset(ResetType);
	Mapper.Write = EMU->GetCPUWriteHandler(0x4);
	EMU->SetCPUWriteHandler(0x4,Write);

	if (ResetType == RESET_HARD)
		Mapper.CHR = 0;
	Sync();
}

static	void	MAPINT	Unload (void)
{
	VS_Unload();
}

static	u8 MapperNum = 99;
CTMapperInfo	MapperInfo_099 =
{
	&MapperNum,
	_T("VS Unisystem"),
	COMPAT_FULL,
	Load,
	Reset,
	Unload,
	VS_CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	VS_Config
};