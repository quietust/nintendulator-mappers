#include	"..\DLL\d_iNES.h"
#include	<stdlib.h>

static	struct
{
	u8 Init;
	u8 *VRAM[8];
}	Mapper = {0};

static	void	_MAPINT	Write (int Bank, int Where, int What)
{
	u8 x;
	u16 i;
	u8 *CHR;
	if (What & 0x02)
		for (x = 0; x < 8; x++)
			memcpy(EMU->GetCHR_Ptr1(x),Mapper.VRAM[x],1024);
	else	for (x = 0; x < 8; x++)
		{
			CHR = EMU->GetCHR_Ptr1(x);
			for (i = 0; i < 0x400; i++)
				CHR[i] = ~Mapper.VRAM[x][i];
		}
	EMU->SetCHR_ROM8(0,0);
}

static	void	_MAPINT	Shutdown (void)
{
	Mapper.Init = 0;
	iNES_UnloadROM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;

	iNES_InitROM();

	for (x = 0x8; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,Write);

	EMU->SetPRG_ROM16(0x8,0);
	EMU->SetPRG_ROM16(0xC,1);
	EMU->SetCHR_RAM8(0,0);
	for (x = 0; x < 8; x++)
		Mapper.VRAM[x] = EMU->GetCHR_Ptr1(x);	/* figure out where CHR_RAM is */
	EMU->SetCHR_ROM8(0,0);
	if (Mapper.Init == 0)
	{
		for (x = 0; x < 8; x++)
			memcpy(Mapper.VRAM[x],EMU->GetCHR_Ptr1(x),1024);
		Mapper.Init = 1;
	}
}

CTMapperInfo	MapperInfo_185 =
{
	185,
	NULL,
	"Mapper 185",
	COMPAT_PARTIAL,
	Reset,
	Shutdown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};