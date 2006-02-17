#include	"..\..\DLL\d_UNIF.h"

static	struct
{
	struct	MSChan
	{
		u8 FIFO[256], ReadPos, WritePos;
		BOOL IsFull, IsEmpty;
		int freq, vol;
		int timer;
		int Pos;
	} Chan[2];
}	MapSound;
static	int	MapperSnd_GenerateWave (struct MSChan *Chan, int Cycles)
{
	int x = 0, y;
	for (y = 0; y < Cycles; y++)
	{
		if (Chan->IsEmpty)
			break;
		if (!Chan->timer--)
		{
			Chan->timer = Chan->freq;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsFull = FALSE;
			Chan->Pos = Chan->FIFO[++Chan->ReadPos] * Chan->vol;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsEmpty = TRUE;
		}
		x += Chan->Pos;
	}
	return x / Cycles;
}
static	int	_MAPINT	MapperSnd (int Cycles)
{
	int out = 0;
	out += MapperSnd_GenerateWave(&MapSound.Chan[0],Cycles);
	out += MapperSnd_GenerateWave(&MapSound.Chan[1],Cycles);
	return out << 3;
}
static	int	_MAPINT	MapperSnd_SaveLoad (int mode, int x, char *data)
{
	int i;
	for (i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].FIFO[i])
	for (i = 0; i < 256; i++)
		SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].FIFO[i])
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].ReadPos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].ReadPos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].WritePos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].WritePos)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].IsFull)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].IsFull)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].IsEmpty)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].IsEmpty)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[0].freq)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[1].freq)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[0].vol)
	SAVELOAD_BYTE(mode,x,data,MapSound.Chan[1].vol)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[0].timer)
	SAVELOAD_WORD(mode,x,data,MapSound.Chan[1].timer)
	return x;
}

static	struct
{
	u8 PRG, CHR[4], Flags;
	u8 IRQenabled, IRQlatch;
	u16 IRQcounter;
	u8 *ExtRam0, *ExtRam1;
	u16 LastAddr, LastAddrTmp;
	FPPURead PPUReadNT[4];
	u8 M[4];
}	Mapper;

int	_MAPINT	ReadNT (int Bank, int Where)
{
	if ((Mapper.Flags & 0x4) && (Where >= 0x3C0) && ((Bank & 3) == ((Mapper.LastAddr >> 10) & 3)))
	{
		const unsigned char AttribBits[4] = {0x00,0x55,0xAA,0xFF};
		if (Mapper.M[Bank & 3])
			return AttribBits[Mapper.ExtRam1[Mapper.LastAddr & 0x3FF]];
		else	return AttribBits[Mapper.ExtRam0[Mapper.LastAddr & 0x3FF]];
	}
	else	return Mapper.PPUReadNT[Bank & 3](Bank,Where);
}

static	void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8,Mapper.PRG);
	EMU->SetPRG_ROM16(0xC,0xF);
	EMU->SetCHR_ROM2(0x0,Mapper.CHR[0]);
	EMU->SetCHR_ROM2(0x2,Mapper.CHR[1]);
	EMU->SetCHR_ROM2(0x4,Mapper.CHR[2]);
	EMU->SetCHR_ROM2(0x6,Mapper.CHR[3]);
	switch (Mapper.Flags & 0x3)
	{
	case 0:	EMU->Mirror_V();	Mapper.M[0] = 0; Mapper.M[1] = 1; Mapper.M[2] = 0; Mapper.M[3] = 1;	break;
	case 1:	EMU->Mirror_H();	Mapper.M[0] = 0; Mapper.M[1] = 0; Mapper.M[2] = 1; Mapper.M[3] = 1;	break;
	case 2:	EMU->Mirror_S0();	Mapper.M[0] = 0; Mapper.M[1] = 0; Mapper.M[2] = 0; Mapper.M[3] = 0;	break;
	case 3:	EMU->Mirror_S1();	Mapper.M[0] = 1; Mapper.M[1] = 1; Mapper.M[2] = 1; Mapper.M[3] = 1;	break;
	}
	if (Mapper.Flags & 0x04)
	{
		EMU->SetPPUReadHandler(0x8,ReadNT);
		EMU->SetPPUReadHandler(0x9,ReadNT);
		EMU->SetPPUReadHandler(0xA,ReadNT);
		EMU->SetPPUReadHandler(0xB,ReadNT);
	}
	else
	{
		EMU->SetPPUReadHandler(0x8,Mapper.PPUReadNT[0]);
		EMU->SetPPUReadHandler(0x9,Mapper.PPUReadNT[1]);
		EMU->SetPPUReadHandler(0xA,Mapper.PPUReadNT[2]);
		EMU->SetPPUReadHandler(0xB,Mapper.PPUReadNT[3]);
	}
	if (Mapper.Flags & 0x8)
		EMU->SetPRG_RAM8(0x6,0);
	else
	{
		EMU->SetPRG_OB4(0x6);
		EMU->SetPRG_OB4(0x7);
	}
}

static	int	_MAPINT	SaveLoad (int mode, int x, char *data)
{
	u8 i;
	SAVELOAD_WORD(mode,x,data,Mapper.IRQcounter)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQenabled)
	SAVELOAD_BYTE(mode,x,data,Mapper.IRQlatch)
	SAVELOAD_BYTE(mode,x,data,Mapper.PRG)
	for (i = 0; i < 4; i++)
		SAVELOAD_BYTE(mode,x,data,Mapper.CHR[i])
	SAVELOAD_BYTE(mode,x,data,Mapper.Flags)
	SAVELOAD_WORD(mode,x,data,Mapper.LastAddr)
	SAVELOAD_WORD(mode,x,data,Mapper.LastAddrTmp)

	if (mode == STATE_LOAD)
		Sync();
	return x;
}

static	void	_MAPINT	CPUCycle (void)
{
	if ((Mapper.IRQenabled) && (!--Mapper.IRQcounter))
		EMU->SetIRQ(0);
}
static	void	_MAPINT	PPUCycle (int Addr, int Scanline, int Cycle, int IsRendering)
{
	Mapper.LastAddr = Mapper.LastAddrTmp;
	Mapper.LastAddrTmp = Addr;
}

static	int	_MAPINT	Read (int Bank, int Where)
{
	struct MSChan *Chan;
	int result = 0;
	if (Where & 4)
		Chan = &MapSound.Chan[0];
	else	Chan = &MapSound.Chan[1];
	if (Chan->IsFull)
		result |= 0x80;
	if (Chan->IsEmpty)
		result |= 0x01;
	return result;
}
static	void	_MAPINT	WriteL (int Bank, int Where, int What)
{
	if (Where & 0x8)
	{
		switch (Where & 0x7)
		{
		case 0:	Mapper.IRQlatch = What;
			break;
		case 1:	Mapper.IRQcounter = ((What & 0x7F) << 8) | Mapper.IRQlatch;
			Mapper.IRQenabled = What & 0x80;
			EMU->SetIRQ(1);
			break;
		case 2:	Mapper.Flags = What & 0xF;
			break;
		case 3:	Mapper.PRG = What & 0xF;
			break;
		case 4:	Mapper.CHR[0] = What & 0xF;
			break;
		case 5:	Mapper.CHR[1] = What & 0xF;
			break;
		case 6:	Mapper.CHR[2] = What & 0xF;
			break;
		case 7:	Mapper.CHR[3] = What & 0xF;
			break;
		}
		Sync();
	}
	else
	{
		struct MSChan *Chan;
		if (Where & 4)
			Chan = &MapSound.Chan[0];
		else	Chan = &MapSound.Chan[1];

		switch (Where & 3)
		{
		case 0x0:
			memset(Chan->FIFO,0,256);
			Chan->ReadPos = Chan->WritePos = 0;
			Chan->IsFull = FALSE;
			Chan->IsEmpty = TRUE;
			Chan->Pos = 0;
			Chan->timer = Chan->freq;
			break;
		case 0x1:
			if (Chan->ReadPos == Chan->WritePos)
			{
				Chan->IsEmpty = FALSE;
				Chan->Pos = What * Chan->vol;
			}
			Chan->FIFO[Chan->WritePos++] = What;
			if (Chan->ReadPos == Chan->WritePos)
				Chan->IsFull = TRUE;
			break;
		case 0x2:
			Chan->freq = (Chan->freq & 0xF00) | What;
			break;
		case 0x3:
			Chan->freq = (Chan->freq & 0xFF) | ((What & 0xF) << 8);
			Chan->vol = (What & 0xF0) >> 4;
			Chan->Pos = Chan->FIFO[Chan->ReadPos] * Chan->vol;
			break;
		}
	}
}
static	void	_MAPINT	WriteH (int Bank, int Where, int What)
{
	if (Where & 0x400)
		Mapper.ExtRam1[Where & 0x3FF] = What & 3;
	else	Mapper.ExtRam0[Where & 0x3FF] = What & 3;
}

static	void	_MAPINT	Shutdown (void)
{
	UNIF_SaveSRAM();
}

static	void	_MAPINT	Reset (int IsHardReset)
{
	u8 x;
	UNIF_InitSRAM(8192);

	EMU->SetCPUReadHandler(0x5,Read);
	for (x = 0x8; x < 0xC; x++)
		EMU->SetCPUWriteHandler(x,WriteL);
	for (x = 0xC; x < 0x10; x++)
		EMU->SetCPUWriteHandler(x,WriteH);

	Mapper.PPUReadNT[0] = EMU->GetPPUReadHandler(0x8);
	Mapper.PPUReadNT[1] = EMU->GetPPUReadHandler(0x9);
	Mapper.PPUReadNT[2] = EMU->GetPPUReadHandler(0xA);
	Mapper.PPUReadNT[3] = EMU->GetPPUReadHandler(0xB);
	
	memset(&MapSound,0,sizeof(MapSound));
	MapSound.Chan[0].IsEmpty = TRUE;
	MapSound.Chan[1].IsEmpty = TRUE;

	Mapper.PRG = 0;
	for (x = 0; x < 4; x++)
		Mapper.CHR[x] = x;
	Mapper.Flags = 0;
	Mapper.IRQenabled = Mapper.IRQlatch = 0;
	Mapper.IRQcounter = 0;
	Mapper.LastAddr = Mapper.LastAddrTmp = 0;
	EMU->Mirror_4();
	Mapper.ExtRam0 = EMU->GetCHR_Ptr1(0xA);
	Mapper.ExtRam1 = EMU->GetCHR_Ptr1(0xB);

	Sync();
}

CTMapperInfo	MapperInfo_UNL_DRIPGAME =
{
	"UNL-DRIPGAME",
	"Drip",
	COMPAT_FULL,
	Reset,
	Shutdown,
	CPUCycle,
	PPUCycle,
	SaveLoad,
	MapperSnd,
	NULL
};
