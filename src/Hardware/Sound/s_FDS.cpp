/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\..\interface.h"
#include	"s_FDS.h"
#include	<math.h>

namespace FDSsound
{
// Sound code borrowed from NEZplug 0.9.4.8

#define FM_DEPTH 0 /* 0,1,2 */
#define NES_BASECYCLES (21477270)
#define PGCPS_BITS (32-16-6)
#define EGCPS_BITS (12)
#define VOL_BITS 12

#define LOG_BITS 12
#define LIN_BITS 7
#define LOG_LIN_BITS 30

struct FDS_EG {
	uint8 spd;
	uint8 cnt;
	uint8 mode;
	uint8 volume;
};
struct FDS_PG {
	uint32 spdbase;
	uint32 spd;
	uint32 freq;
};
struct FDS_WG {
	uint32 phase;
	int8 wave[0x40];
	uint8 wavptr;
	int8 output;
	uint8 disable;
	uint8 disable2;
};
struct FDS_OP {
	struct FDS_EG eg;
	struct FDS_PG pg;
	struct FDS_WG wg;
	uint8 bias;
	uint8 wavebase;
	uint8 d[2];
};

struct FDS_OP op[2];
uint32 phasecps;
uint32 envcnt;
uint32 envspd;
uint32 envcps;
uint8 envdisable;
uint8 d[3];
uint32 lvl;
int32 mastervolumel[4];
uint32 mastervolume;
uint32 srate;
uint8 reg[0x10];

static uint32 lineartbl[(1 << LIN_BITS) + 1];
static uint32 logtbl[1 << LOG_BITS];
/*
static uint32 LinearToLog(int32 l)
{
	return (l < 0) ? (lineartbl[-l] + 1) : lineartbl[l];
}
*/
static int32 LogToLinear(uint32 l, uint32 sft)
{
	int32 ret;
	uint32 ofs;
	l += sft << (LOG_BITS + 1);
	sft = l >> (LOG_BITS + 1);
	if (sft >= LOG_LIN_BITS) return 0;
	ofs = (l >> 1) & ((1 << LOG_BITS) - 1);
	ret = logtbl[ofs] >> sft;
	return (l & 1) ? -ret : ret;
}

static void LogTableInitialize(void)
{
	static volatile uint32 initialized = 0;
	uint32 i;
	double a;
	if (initialized) return;
	initialized = 1;
	for (i = 0; i < (1 << LOG_BITS); i++)
	{
		a = (1 << LOG_LIN_BITS) / pow(2.0, i / (double)(1 << LOG_BITS));
		logtbl[i] = (uint32)a;
	}
	lineartbl[0] = LOG_LIN_BITS << LOG_BITS;
	for (i = 1; i < (1 << LIN_BITS) + 1; i++)
	{
		uint32 ua;
		a = i << (LOG_LIN_BITS - LIN_BITS);
		ua = (uint32)((LOG_LIN_BITS - (log(a) / log(2.0))) * (1 << LOG_BITS));
		lineartbl[i] = ua << 1;
	}
}

static void FDSSoundWGStep(struct FDS_WG *pwg)
{
	if (pwg->disable || pwg->disable2) return;
	pwg->output = pwg->wave[(pwg->phase >> (PGCPS_BITS+16)) & 0x3f];
}

static void FDSSoundEGStep(struct FDS_EG *peg)
{
	if (peg->mode & 0x80) return;
	if (++peg->cnt <= peg->spd) return;
	peg->cnt = 0;
	if (peg->mode & 0x40)
		peg->volume += (peg->volume < 0x1f);
	else
		peg->volume -= (peg->volume > 0);
}


static int32 __fastcall FDSSoundRender(void)
{
	int32 output;
	/* Wave Generator */
	FDSSoundWGStep(&op[1].wg);
	FDSSoundWGStep(&op[0].wg);

	/* Frequency Modulator */
	op[1].pg.spd = op[1].pg.spdbase;
	if (op[1].wg.disable)
		op[0].pg.spd = op[0].pg.spdbase;
	else
	{
		uint32 v1;
		v1 = 0x10000 + ((int32)op[1].eg.volume) * (((int32)(((uint8)op[1].wg.output) & 255)) - 64);
		v1 = ((1 << 10) + v1) & 0xfff;
		v1 = (op[0].pg.freq * v1) >> 10;
		op[0].pg.spd = v1 * phasecps;
	}

	/* Accumulator */
	output = op[0].eg.volume;
	if (output > 0x20) output = 0x20;
	output = (op[0].wg.output * output * mastervolumel[lvl]) >> (VOL_BITS - 4);

	/* Envelope Generator */
	if (!envdisable && envspd)
	{
		envcnt += envcps;
		while (envcnt >= envspd)
		{
			envcnt -= envspd;
			FDSSoundEGStep(&op[1].eg);
			FDSSoundEGStep(&op[0].eg);
		}
	}

	/* Phase Generator */
	op[1].wg.phase += op[1].pg.spd;
	op[0].wg.phase += op[0].pg.spd;
	return (op[0].pg.freq != 0) ? output : 0;
}

static void __fastcall FDSSoundVolume(unsigned int volume)
{
	volume += 196;
	mastervolume = (volume << (LOG_BITS - 8)) << 1;
	mastervolumel[0] = LogToLinear(mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2;
	mastervolumel[1] = LogToLinear(mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 4 / 3;
	mastervolumel[2] = LogToLinear(mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2 / 2;
	mastervolumel[3] = LogToLinear(mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 8 / 10;
}

static const uint8 wave_delta_table[8] = {
	0,(1 << FM_DEPTH),(2 << FM_DEPTH),(4 << FM_DEPTH),
	0,256 - (4 << FM_DEPTH),256 - (2 << FM_DEPTH),256 - (1 << FM_DEPTH),
};

static uint32 DivFix(uint32 p1, uint32 p2, uint32 fix)
{
	uint32 ret;
	ret = p1 / p2;
	p1  = p1 % p2;
	while (fix--)
	{
		p1 += p1;
		ret += ret;
		if (p1 >= p2)
		{
			p1 -= p2;
			ret++;
		}
	}
	return ret;
}

void	Load (void)
{
	ZeroMemory(op, sizeof(op));
	envcnt = 0;
	ZeroMemory(d, sizeof(d));
	lvl = 0;
	ZeroMemory(reg, sizeof(reg));

	srate = 44100;
	envcps = DivFix(NES_BASECYCLES, 12 * srate, EGCPS_BITS + 5 - 9 + 1);
	envspd = 0xe8 << EGCPS_BITS;
	envdisable = 1;
	phasecps = DivFix(NES_BASECYCLES, 12 * srate, PGCPS_BITS);
	LogTableInitialize();
	FDSSoundVolume(0);
}

void	Reset (RESET_TYPE ResetType)
{
	int i;
	for (i = 0; i < 0x40; i++)
	{
		op[0].wg.wave[i] = (i < 0x20) ? 0x1F : -0x20;
		op[1].wg.wave[i] = 64;
	}
}

void	Unload (void)
{
}

int	Read (int Addr)
{
	if ((0x4040 <= Addr) && (Addr <= 0x407F))
		return op[0].wg.wave[Addr & 0x3F] + 0x20;
	if (0x4090 == Addr)
		return op[0].eg.volume | 0x40;
	if (0x4092 == Addr) /* 4094? */
		return op[1].eg.volume | 0x40;
	return -1;
}

void	Write (int Addr, int Val)
{
	if (0x4040 <= Addr && Addr <= 0x407F)
	{
		op[0].wg.wave[Addr - 0x4040] = ((int)(Val & 0x3F)) - 0x20;
	}
	else if (0x4080 <= Addr && Addr <= 0x408F)
	{
		struct FDS_OP *pop = &op[(Addr & 4) >> 2];
		reg[Addr - 0x4080] = Val;
		switch (Addr & 0xf)
		{
			case 0:
			case 4:
				pop->eg.mode = Val & 0xc0;
				if (pop->eg.mode & 0x80)
				{
					pop->eg.volume = (Val & 0x3f);
				}
				else
				{
					pop->eg.spd = Val & 0x3f;
				}
				break;
			case 5:
				op[1].bias = Val & 255;
				break;
			case 2:	case 6:
				pop->pg.freq &= 0x00000F00;
				pop->pg.freq |= (Val & 0xFF) << 0;
				pop->pg.spdbase = pop->pg.freq * phasecps;
				break;
			case 3:
				envdisable = Val & 0x40;
			case 7:
				pop->pg.freq &= 0x000000FF;
				pop->pg.freq |= (Val & 0x0F) << 8;
				pop->pg.spdbase = pop->pg.freq * phasecps;
				pop->wg.disable = Val & 0x80;
				if (pop->wg.disable)
				{
					pop->wg.phase = 0;
					pop->wg.wavptr = 0;
					pop->wavebase = 0;
				}
				break;
			case 8:
				if (op[1].wg.disable)
				{
					int32 idx = Val & 7;
					if (idx == 4)
					{
						op[1].wavebase = 0;
					}
					op[1].wavebase += wave_delta_table[idx];
					op[1].wg.wave[op[1].wg.wavptr + 0] = (op[1].wavebase + op[1].bias + 64) & 255;
					op[1].wavebase += wave_delta_table[idx];
					op[1].wg.wave[op[1].wg.wavptr + 1] = (op[1].wavebase + op[1].bias + 64) & 255;
					op[1].wg.wavptr = (op[1].wg.wavptr + 2) & 0x3f;
				}
				break;
			case 9:
				lvl = (Val & 3);
				op[0].wg.disable2 = Val & 0x80;
				break;
			case 10:
				envspd = Val << EGCPS_BITS;
				break;
		}
	}
}

int	MAPINT	Get (int numCycles)
{
	return FDSSoundRender() >> 9;	// current code does not run per-cycle
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	// TODO - save and load FDS sound state
	return offset;
}
} // namespace FDSsound