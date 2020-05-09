/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\interface.h"
#include	"s_MMC5.h"

// Nintendo MMC5, based on the NES APU
namespace MMC5sound
{
#define	QUARTER_FRAME_LEN	7457

const	unsigned char	LengthCounts[32] = {
	0x0A,0xFE,
	0x14,0x02,
	0x28,0x04,
	0x50,0x06,
	0xA0,0x08,
	0x3C,0x0A,
	0x0E,0x0C,
	0x1A,0x0E,

	0x0C,0x10,
	0x18,0x12,
	0x30,0x14,
	0x60,0x16,
	0xC0,0x18,
	0x48,0x1A,
	0x10,0x1C,
	0x20,0x1E
};
const	signed char	SquareDuty[4][8] = {
	{-4,-4,-4,-4,-4,-4,-4,+4},
	{-4,-4,-4,-4,-4,-4,+4,+4},
	{-4,-4,-4,-4,+4,+4,+4,+4},
	{+4,+4,+4,+4,+4,+4,-4,-4},
};

struct	MMC5Sqr
{
	unsigned char volume, envelope, wavehold, duty;
	unsigned long freq;
	unsigned char Vol;
	unsigned char CurD;
	unsigned char LengthCtr;
	unsigned char EnvCtr, Envelope;
	BOOL Enabled, ValidFreq, Active;
	BOOL EnvClk;
	signed long Cycles;
	signed long Pos;
	signed long FrameCycles;

	void	CheckActive ()
	{
		ValidFreq = (freq > 1);	// JustBreed sets Freq=1 in places, which sounds very bad due to lack of downsampling
		Active = LengthCtr && ValidFreq;
		Pos = Active ? (SquareDuty[duty][CurD] * Vol) : 0;
	}
	void	Reset ()
	{
		volume = envelope = wavehold = duty = 0;
		freq = 0;
		Vol = 0;
		CurD = 0;
		LengthCtr = 0;
		Envelope = 0;
		Enabled = ValidFreq = Active = FALSE;
		EnvClk = FALSE;
		Pos = 0;
		Cycles = 1;
		EnvCtr = 1;
		FrameCycles = 0;
	}
	void	Write (int Reg, int Val)
	{
		switch (Reg)
		{
		case 0:	volume = Val & 0xF;
			envelope = Val & 0x10;
			wavehold = Val & 0x20;
			duty = (Val >> 6) & 0x3;
			Vol = envelope ? volume : Envelope;
			break;
		case 2:	freq &= 0x700;
			freq |= Val;
			break;
		case 3:	freq &= 0xFF;
			freq |= (Val & 0x7) << 8;
			if (Enabled)
				LengthCtr = LengthCounts[(Val >> 3) & 0x1F];
			CurD = 0;
			EnvClk = TRUE;
			break;
		case 4:	Enabled = Val ? TRUE : FALSE;
			if (!Enabled)
				LengthCtr = 0;
			break;
		}
		CheckActive();
	}
	int	Generate (int cycles)
	{
		// Step 1: run the equivalent of the Frame Timer
		FrameCycles -= cycles;
		while (FrameCycles <= 0)
		{
			FrameCycles += QUARTER_FRAME_LEN;
			if (EnvClk)
			{
				EnvClk = FALSE;
				Envelope = 0xF;
				EnvCtr = volume;
			}
			else if (!--EnvCtr)
			{
				EnvCtr = volume;
				if (Envelope)
					Envelope--;
				else	Envelope = wavehold ? 0xF : 0x0;
			}

			if (LengthCtr && !wavehold)
				LengthCtr--;
		}
		Vol = envelope ? volume : Envelope;
		CheckActive();

		// Step 2: run the actual channel itself
		Cycles -= cycles;
		// Changed to "freq + 1" and "<= 0" to avoid infinite loop when value is zero
		while (Cycles <= 0)
		{
			Cycles += (freq + 1) << 1;
			CurD = (CurD - 1) & 0x7;
			if (Active)
				Pos = SquareDuty[duty][CurD] * Vol;
		}

		return Pos;
	}
} Sqr0, Sqr1;
unsigned char byte10;
unsigned char IRQreads;
int PCM;
FCPURead _CPURead[4];

BOOL	HaveIRQ (void)
{
	if (byte10 & IRQreads & 0x80)
		return TRUE;
	else	return FALSE;
}
int	MAPINT	CPUReadPCM (int Bank, int Addr);

void	Load (void)
{
	PCM = 0;
	Reset(RESET_HARD);
}

void	Reset (RESET_TYPE ResetType)
{
	Sqr0.Reset();
	Sqr1.Reset();
	IRQreads = 0;
	byte10 = 0;
}

void	Unload (void)
{
}

void	Write (int Addr, int Val)
{
	switch (Addr)
	{
	case 0x5000:	Sqr0.Write(0, Val);	break;
	case 0x5002:	Sqr0.Write(2, Val);	break;
	case 0x5003:	Sqr0.Write(3, Val);	break;
	case 0x5004:	Sqr1.Write(0, Val);	break;
	case 0x5006:	Sqr1.Write(2, Val);	break;
	case 0x5007:	Sqr1.Write(3, Val);	break;
	case 0x5010:	if ((byte10 ^ Val) & 0x01)
			{
				for (int i = 0; i < 4; i++)
				{
					if (Val & 0x01)
					{
						_CPURead[i] = EMU->GetCPUReadHandler(0x8 | i);
						EMU->SetCPUReadHandler(0x8 | i, CPUReadPCM);
						// Important assumption: the old handler was plain ROM/RAM and is thus debug-safe
						EMU->SetCPUReadHandlerDebug(0x8 | i, _CPURead[i]);
					}
					else	EMU->SetCPUReadHandler(0x8 | i, _CPURead[i]);
				}
			}
			byte10 = Val;
			break;
	case 0x5011:	if (!(byte10 & 0x01))
				PCM = Val;
			if (PCM == 0)
				IRQreads = 0x80;
			break;
	case 0x5015:	Sqr0.Write(4, Val & 0x01);
			Sqr1.Write(4, Val & 0x02);
			break;
	}
}

int	Read (int Addr)
{
	int read = -1;
	switch (Addr)
	{
	case 0x5010:	read = IRQreads;
			IRQreads = 0;
			break;
	case 0x5015:	read = ((Sqr0.LengthCtr) ? 1 : 0) | ((Sqr1.LengthCtr) ? 2 : 0);
			break;
	}
	return read;
}

int	MAPINT	CPUReadPCM (int Bank, int Addr)
{
	int Val = _CPURead[Bank - 8](Bank,Addr);
	if (byte10 & 0x01)
	{
		PCM = Val;
		if (PCM == 0)
			IRQreads = 0x80;
	}
	return Val;
}

int	MAPINT	Get (int Cycles)
{
	int z = 0;
	z += Sqr0.Generate(Cycles);
	z += Sqr1.Generate(Cycles);
	z += PCM;
	return z << 6;
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_BYTE(mode, offset, data, Sqr0.volume);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.envelope);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.wavehold);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.duty);
	SAVELOAD_LONG(mode, offset, data, Sqr0.freq);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.Vol);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.CurD);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.LengthCtr);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.EnvCtr);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.Envelope);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.Enabled);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.ValidFreq);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.Active);
	SAVELOAD_BYTE(mode, offset, data, Sqr0.EnvClk);
	SAVELOAD_LONG(mode, offset, data, Sqr0.Cycles);
	SAVELOAD_LONG(mode, offset, data, Sqr0.Pos);
	SAVELOAD_LONG(mode, offset, data, Sqr0.FrameCycles);

	SAVELOAD_BYTE(mode, offset, data, Sqr1.volume);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.envelope);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.wavehold);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.duty);
	SAVELOAD_LONG(mode, offset, data, Sqr1.freq);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.Vol);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.CurD);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.LengthCtr);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.EnvCtr);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.Envelope);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.Enabled);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.ValidFreq);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.Active);
	SAVELOAD_BYTE(mode, offset, data, Sqr1.EnvClk);
	SAVELOAD_LONG(mode, offset, data, Sqr1.Cycles);
	SAVELOAD_LONG(mode, offset, data, Sqr1.Pos);
	SAVELOAD_LONG(mode, offset, data, Sqr1.FrameCycles);

	SAVELOAD_BYTE(mode, offset, data, byte10);
	SAVELOAD_BYTE(mode, offset, data, PCM);
	SAVELOAD_BYTE(mode, offset, data, IRQreads);
	return offset;
}
} // namespace MMC5sound
