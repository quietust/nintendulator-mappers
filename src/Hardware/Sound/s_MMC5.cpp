/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
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
const	signed char	Duties[4][8] = {
	{-4,+4,-4,-4,-4,-4,-4,-4},
	{-4,+4,+4,-4,-4,-4,-4,-4},
	{-4,+4,+4,+4,+4,-4,-4,-4},
	{+4,-4,-4,+4,+4,+4,+4,+4}
};

struct	MMC5Sqr
{
	unsigned char volume, envelope, wavehold, duty;
	unsigned long freq;
	int Vol;
	unsigned char CurD;
	int Timer;
	int EnvCtr, Envelope;
	BOOL Enabled, ValidFreq, Active;
	BOOL EnvClk;
	int Cycles;
	signed long Pos;
	unsigned long FrameCycles;

	void	CheckActive ()
	{
		ValidFreq = (freq >= 0x8);
		if ((ValidFreq) && (Timer))
		{
			Active = TRUE;
			Pos = Duties[duty][CurD] * Vol;
		}
		else
		{
			Active = FALSE;
			Pos = 0;
		}
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
				Timer = LengthCounts[(Val >> 3) & 0x1F] << 1;
			CurD = 0;
			EnvClk = TRUE;
			break;
		case 4:	Enabled = Val ? TRUE : FALSE;
			if (!Enabled)
				Timer = 0;
			break;
		}
		CheckActive();
	}
	int	Generate (int cycles)
	{
		if (!Active)
			return 0;
		Cycles -= cycles;
		while (Cycles <= 0)
		{
			Cycles += (freq + 1) << 1;
			CurD = (CurD + 1) & 0x7;
			if (Active)
				Pos = Duties[duty][CurD] * Vol;
		}

		FrameCycles -= cycles;
		while (FrameCycles <= 0)
		{
			FrameCycles += QUARTER_FRAME_LEN;
			if (EnvClk)
			{
				EnvClk = FALSE;
				Envelope = 0xF;
				EnvCtr = volume + 1;
			}
			else if (!--EnvCtr)
			{
				EnvCtr = volume + 1;
				if (Envelope)
					Envelope--;
				else	Envelope = wavehold ? 0xF : 0x0;
			}

			if (Timer && !wavehold)
				Timer--;
		}
		Vol = envelope ? volume : Envelope;
		CheckActive();
		return Pos;
	}
} Sqr0, Sqr1;
unsigned char byte0, byte2, byte3, byte4, byte6, byte7, byte10, byte11, byte15;
int PCM;

void	Load (void)
{
	ZeroMemory(&Sqr0, sizeof(Sqr0));
	ZeroMemory(&Sqr1, sizeof(Sqr1));

	byte0 = byte2 = byte3 = byte4 = byte6 = byte7 = byte10 = byte11 = byte15 = 0;
	PCM = 0;
}

void	Reset (RESET_TYPE ResetType)
{
	Sqr0.Cycles = 1;
	Sqr1.Cycles = 1;
}

void	Unload (void)
{
}

void	Write (int Addr, int Val)
{
	switch (Addr)
	{
	case 0x5000:	Sqr0.Write(0, byte0 = Val);	break;
	case 0x5002:	Sqr0.Write(2, byte2 = Val);	break;
	case 0x5003:	Sqr0.Write(3, byte3 = Val);	break;
	case 0x5004:	Sqr1.Write(0, byte4 = Val);	break;
	case 0x5006:	Sqr1.Write(2, byte6 = Val);	break;
	case 0x5007:	Sqr1.Write(3, byte7 = Val);	break;
	case 0x5010:	byte10 = Val;
//			EMU->DbgOut("MMC5 - $5010 set to %02X", Val);
			break;
	case 0x5011:	PCM = byte11 = Val;
			break;
	case 0x5015:	byte15 = Val;
			Sqr0.Write(4, Val & 0x01);
			Sqr1.Write(4, Val & 0x02);
			break;
	}
}

int	Read (int Addr)
{
	int read = -1;
	switch (Addr)
	{
	case 0x5010:	read = PCM;	break;
	case 0x5015:	read = ((Sqr0.Timer) ? 1 : 0) | ((Sqr1.Timer) ? 2 : 0);	break;
	}
	return read;
}

int	MAPINT	Get (int Cycles)
{
	int z = 0;
	if (Sqr0.Enabled)	z += Sqr0.Generate(Cycles);
	if (Sqr1.Enabled)	z += Sqr1.Generate(Cycles);
	z += PCM;
	return z << 6;
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	if (mode == STATE_SAVE)
	{
		memcpy(data + x, &Sqr0, sizeof(Sqr0));
		x += sizeof(Sqr0);
		memcpy(data + x, &Sqr1, sizeof(Sqr1));
		x += sizeof(Sqr1);
	}
	else if (mode == STATE_LOAD)
	{
		memcpy(&Sqr0, data + x, sizeof(Sqr0));
		x += sizeof(Sqr0);
		memcpy(&Sqr1, data + x, sizeof(Sqr1));
		x += sizeof(Sqr1);
	}
	else if (mode == STATE_SIZE)
	{
		x += sizeof(Sqr0);
		x += sizeof(Sqr1);
	}
	else	MessageBox(hWnd, _T("Invalid save/load type!"), _T(__FILE__), MB_OK);

	SAVELOAD_BYTE(mode, x, data, byte0);
	SAVELOAD_BYTE(mode, x, data, byte2);
	SAVELOAD_BYTE(mode, x, data, byte3);
	SAVELOAD_BYTE(mode, x, data, byte4);
	SAVELOAD_BYTE(mode, x, data, byte6);
	SAVELOAD_BYTE(mode, x, data, byte7);
	SAVELOAD_BYTE(mode, x, data, byte10);
	SAVELOAD_BYTE(mode, x, data, byte11);
	SAVELOAD_BYTE(mode, x, data, byte15);
	SAVELOAD_BYTE(mode, x, data, PCM);
	return x;
}
} // namespace MMC5sound