/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\..\interface.h"
#include	"s_N163.h"

// Turn on "serial" audio output to more closely emulate actual hardware
// In practice, this sounds really bad without a lowpass filter
//#define N163_SERIAL

// Namco 163
namespace N163sound
{
uint8_t regs[0x80];
uint8_t addr, step;
int8_t out;
#ifndef N163_SERIAL
int8_t outs[8];
#endif
int volmult;

void	Load (void)
{
	ZeroMemory(regs, sizeof(regs));
	addr = 0;
	volmult = 4;
}

void	Reset (RESET_TYPE ResetType)
{
	if (ResetType == RESET_HARD)
	{
		ZeroMemory(regs, sizeof(regs));
		addr = 0;
	}
	step = 0x7E;
	out = 0;
#ifndef N163_SERIAL
	for (int i = 0; i < 8; i++)
		outs[i] = 0;
#endif
}

void	Unload (void)
{
}

void	Write (int Addr, int Val)
{
	switch (Addr & 0xF800)
	{
	case 0xF800:
		addr = Val;
		break;
	case 0x4800:
		regs[addr & 0x7F] = Val;
		if (addr & 0x80)
			addr = (addr + 1) | 0x80;
		break;
	}
}

int	Read (int Addr)
{
	int data = regs[addr & 0x7F];

	if (addr & 0x80)
		addr = (addr + 1) | 0x80;

	return data;
}

inline void addOverflow (uint8_t &s, uint8_t &a, uint8_t b)
{
	int sum = (uint32_t)a + (uint32_t)b + ((s & 0x80) ? 1 : 0);
	a = sum & 0xFF;
	if (sum > 0xFF)
		s |= 0x80;
	else	s &= 0x7F;
}

int getChannelPos(int channel)
{
	int chan = 0x40 | (channel << 3);
	uint8_t sampaddr = (regs[chan+6] + (regs[chan+5] - (regs[chan+4] & 0xFC))) & 0xFF;
	uint8_t p = regs[sampaddr >> 1];
	if (sampaddr & 1)
		p >>= 4;
	else	p &= 0xF;
	return ((int)p - 8) * (regs[chan+7] & 0xF);
}
int	MAPINT	Get (int Cycles)
{
	int numChans = ((regs[0x7F] >> 4) & 0x7) + 1;
	int output = 0;
	for (int i = 0; i < Cycles; i++)
	{
		int chan = 0x40 | ((step & 0x70) >> 1);
		switch (step & 0xF)
		{
		// Actual timings are approximate
		case 4:
			addOverflow(step, regs[chan+1], regs[chan+0]);
			break;
		case 8:
			addOverflow(step, regs[chan+3], regs[chan+2]);
			break;
		case 12:
			addOverflow(step, regs[chan+5], regs[chan+4] & 0x3);
			// Handle carry output and length underflow at the same time
			if ((regs[chan + 5] & 0xFC) < (regs[chan + 4] & 0xFC))
				regs[chan + 5] += regs[chan + 4] & 0xFC;
			break;
		case 14:
			out = getChannelPos((chan >> 3) & 0x7);
#ifndef N163_SERIAL
			outs[(chan >> 3) & 7] = out;
#endif
			// There are only 15 states, but the increment below by itself would give us 16
			// So increment once more here, and mask off the carry bit to simplify the logic below
			step = (step + 1) & 0x7F;
			break;
		}
		if (step == 0x7F)
			step = ~regs[0x7F] & 0x70;
		else	step++;
#ifdef	N163_SERIAL
		output += out;
#else
		// In parallel mode, accumulate all 8 channels on every cycle
		// to try and reduce aliasing artifacts
		for (int j = 8 - numChans; j < 8; j++)
			output += outs[j];
#endif
	}
#ifdef	N163_SERIAL
	return (output << 6) * volmult / Cycles;
#else
	return (output << 6) * volmult / Cycles / numChans;
#endif
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 1;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	for (int i = 0; i < 0x80; i++)
		SAVELOAD_BYTE(mode, offset, data, regs[i]);
	SAVELOAD_BYTE(mode, offset, data, addr);

	if (ver == 1)
	{
		SAVELOAD_BYTE(mode, offset, data, step);
		SAVELOAD_BYTE(mode, offset, data, out);
	}

	// Skip data from old states
	if (ver == 0)
		offset += 48;

	return offset;
}

void	SetVolume (int mult)
{
	volmult = mult;
}

// Save internal memory to SRAM file
void	SaveSRAM (int offset)
{
	EMU->SetPRG_RAM4(0x6, 0);
	memcpy(EMU->GetPRG_Ptr4(0x6) + offset, regs, 128);
	EMU->SetPRG_OB4(0x6);
}
// Load internal memory from SRAM file
void	LoadSRAM (int offset)
{
	EMU->SetPRG_RAM4(0x6, 0);
	memcpy(regs, EMU->GetPRG_Ptr4(0x6) + offset, 128);
	EMU->SetPRG_OB4(0x6);
}
} // namespace N163sound
