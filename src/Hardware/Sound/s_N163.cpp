/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\interface.h"
#include	"s_N163.h"

// Namco 163
namespace N163sound
{
uint8_t regs[0x80];
uint8_t chans;
uint8_t addr;
uint8_t inc;
struct	N163chan
{
	uint8_t freql, freqm, freqh;
	uint32_t freq;
	uint16_t len;
	uint8_t baseaddr;
	uint8_t volume;
	uint8_t CurP;
	uint8_t CurA;
	int32_t LCtr;

	int	GenerateWave (int Cycles)
	{
		int32_t _freq;
		if (!freq)
			return 0;
		_freq = (0xF0000 * chans) / freq;
		LCtr += Cycles;
		while (LCtr > _freq)
		{
			uint8_t _addr;
			CurA++;
			while (CurA >= len)
				CurA -= len;
			_addr = baseaddr + CurA;
			CurP = regs[_addr >> 1];
			if (_addr & 1)
				CurP >>= 4;
			else	CurP &= 0xF;
			LCtr -= _freq;
		}
		return (CurP - 0x8) * volume;
	}
	void	Write (int Addr, int Val)
	{
		switch (Addr & 0x7)
		{
		case 0:	freql = Val;
			freq = freql | (freqm << 8) | (freqh << 16);
			break;
		case 2:	freqm = Val;
			freq = freql | (freqm << 8) | (freqh << 16);
			break;
		case 4:	freqh = Val & 3;
			freq = freql | (freqm << 8) | (freqh << 16);
			if (len != 0x100 - (Val & 0xFC))
			{
				len = 0x100 - (Val & 0xFC);
				CurA = 0;
			}
			break;
		case 6:	baseaddr = Val;
			break;
		case 7:	volume = Val & 0xF;
			break;
		}
	}
} Ch[8];

void	Load (void)
{
	ZeroMemory(Ch, sizeof(Ch));
	ZeroMemory(regs, sizeof(regs));
	chans = 0;
	addr = 0;
	inc = 0;
}

void	Reset (RESET_TYPE ResetType)
{
	for (int i = 0; i < 8; i++)
		Ch[i].len = 0x10;
	chans = 8;
	inc = 0x80;
}

void	Unload (void)
{
}

void	Write (int Addr, int Val)
{
	switch (Addr & 0xF800)
	{
	case 0xF800:
		addr = Val & 0x7F;
		inc = Val & 0x80;
		break;
	case 0x4800:
		regs[addr] = Val;
		if (addr & 0x40)
		{
			Ch[(addr & 0x38) >> 3].Write(addr & 0x7, Val);
			if (addr == 0x7F)
				chans = 1 + ((Val >> 4) & 0x7);
		}
		if (inc)
		{
			addr++;
			addr &= 0x7F;
		}
		break;
	}
}

int	Read (int Addr)
{
	int data = regs[addr];

	if (inc)
	{
		addr++;
		addr &= 0x7F;
	}

	return data;
}

int	MAPINT	Get (int Cycles)
{
	int out = 0;
	for (int i = 8 - chans; i < 8; i++)
		out += Ch[i].GenerateWave(Cycles);
	return out << 5;
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	switch (mode)
	{
	case STATE_SAVE:
		for (int i = 0; i < 0x80; i++)
			data[offset++] = regs[i];
		data[offset++] = addr | inc;
		break;
	case STATE_LOAD:
		Write(0xF800, 0x80);
		for (int i = 0; i < 0x80; i++)
			Write(0x4800, data[offset++]);
		Write(0xF800, data[offset++]);
		break;
	case STATE_SIZE:
		offset += 0x81;
		break;
	}
	for (int i = 0; i < 8; i++)
	{
		SAVELOAD_BYTE(mode, offset, data, Ch[i].CurP);
		SAVELOAD_BYTE(mode, offset, data, Ch[i].CurA);
		SAVELOAD_LONG(mode, offset, data, Ch[i].LCtr);
	}
	return offset;
}
} // namespace N163sound
