#include	"..\..\interface.h"
#include	"s_FDS.h"

// Sound code borrowed from NEZplug 0.9.4.8

#define FM_DEPTH 0 /* 0,1,2 */
#define NES_BASECYCLES (21477270)
#define PGCPS_BITS (32-16-6)
#define EGCPS_BITS (12)
#define VOL_BITS 12

#define LOG_BITS 12
#define LIN_BITS 7
#define LOG_LIN_BITS 30

typedef struct FDSsound {
	struct FDS_OP {
		struct FDS_EG {
			u8 spd;
			u8 cnt;
			u8 mode;
			u8 volume;
		} eg;
		struct FDS_PG {
			u32 spdbase;
			u32 spd;
			u32 freq;
		} pg;
		struct FDS_WG {
			u32 phase;
			s8 wave[0x40];
			u8 wavptr;
			s8 output;
			u8 disable;
			u8 disable2;
		} wg;
		u8 bias;
		u8 wavebase;
		u8 d[2];
	}	op[2];
	u32 phasecps;
	u32 envcnt;
	u32 envspd;
	u32 envcps;
	u8 envdisable;
	u8 d[3];
	u32 lvl;
	s32 mastervolumel[4];
	u32 mastervolume;
	u32 srate;
	u8 reg[0x10];
} TFDSsound, *PFDSsound;

static	TFDSsound	FDSsound;

static u32 lineartbl[(1 << LIN_BITS) + 1];
static u32 logtbl[1 << LOG_BITS];
static u32 LinearToLog(s32 l)
{
	return (l < 0) ? (lineartbl[-l] + 1) : lineartbl[l];
}

static s32 LogToLinear(u32 l, u32 sft)
{
	s32 ret;
	u32 ofs;
	l += sft << (LOG_BITS + 1);
	sft = l >> (LOG_BITS + 1);
	if (sft >= LOG_LIN_BITS) return 0;
	ofs = (l >> 1) & ((1 << LOG_BITS) - 1);
	ret = logtbl[ofs] >> sft;
	return (l & 1) ? -ret : ret;
}

static void LogTableInitialize(void)
{
	static volatile u32 initialized = 0;
	u32 i;
	double a;
	if (initialized) return;
	initialized = 1;
	for (i = 0; i < (1 << LOG_BITS); i++)
	{
		a = (1 << LOG_LIN_BITS) / pow(2, i / (double)(1 << LOG_BITS));
		logtbl[i] = (u32)a;
	}
	lineartbl[0] = LOG_LIN_BITS << LOG_BITS;
	for (i = 1; i < (1 << LIN_BITS) + 1; i++)
	{
		u32 ua;
		a = i << (LOG_LIN_BITS - LIN_BITS);
		ua = (u32)((LOG_LIN_BITS - (log(a) / log(2))) * (1 << LOG_BITS));
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


static s32 __fastcall FDSSoundRender(void)
{
	s32 output;
	/* Wave Generator */
	FDSSoundWGStep(&FDSsound.op[1].wg);
	FDSSoundWGStep(&FDSsound.op[0].wg);

	/* Frequency Modulator */
	FDSsound.op[1].pg.spd = FDSsound.op[1].pg.spdbase;
	if (FDSsound.op[1].wg.disable)
		FDSsound.op[0].pg.spd = FDSsound.op[0].pg.spdbase;
	else
	{
		u32 v1;
		v1 = 0x10000 + ((s32)FDSsound.op[1].eg.volume) * (((s32)(((u8)FDSsound.op[1].wg.output) & 255)) - 64);
		v1 = ((1 << 10) + v1) & 0xfff;
		v1 = (FDSsound.op[0].pg.freq * v1) >> 10;
		FDSsound.op[0].pg.spd = v1 * FDSsound.phasecps;
	}

	/* Accumulator */
	output = FDSsound.op[0].eg.volume;
	if (output > 0x20) output = 0x20;
	output = (FDSsound.op[0].wg.output * output * FDSsound.mastervolumel[FDSsound.lvl]) >> (VOL_BITS - 4);

	/* Envelope Generator */
	if (!FDSsound.envdisable && FDSsound.envspd)
	{
		FDSsound.envcnt += FDSsound.envcps;
		while (FDSsound.envcnt >= FDSsound.envspd)
		{
			FDSsound.envcnt -= FDSsound.envspd;
			FDSSoundEGStep(&FDSsound.op[1].eg);
			FDSSoundEGStep(&FDSsound.op[0].eg);
		}
	}

	/* Phase Generator */
	FDSsound.op[1].wg.phase += FDSsound.op[1].pg.spd;
	FDSsound.op[0].wg.phase += FDSsound.op[0].pg.spd;
	return (FDSsound.op[0].pg.freq != 0) ? output : 0;
}

static void __fastcall FDSSoundVolume(unsigned int volume)
{
	volume += 196;
	FDSsound.mastervolume = (volume << (LOG_BITS - 8)) << 1;
	FDSsound.mastervolumel[0] = LogToLinear(FDSsound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2;
	FDSsound.mastervolumel[1] = LogToLinear(FDSsound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 4 / 3;
	FDSsound.mastervolumel[2] = LogToLinear(FDSsound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 2 / 2;
	FDSsound.mastervolumel[3] = LogToLinear(FDSsound.mastervolume, LOG_LIN_BITS - LIN_BITS - VOL_BITS) * 8 / 10;
}

static const u8 wave_delta_table[8] = {
	0,(1 << FM_DEPTH),(2 << FM_DEPTH),(4 << FM_DEPTH),
	0,256 - (4 << FM_DEPTH),256 - (2 << FM_DEPTH),256 - (1 << FM_DEPTH),
};

static u32 DivFix(u32 p1, u32 p2, u32 fix)
{
	u32 ret;
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

void	FDSsound_Init (void)
{
	int i;
	memset(&FDSsound, 0, sizeof(TFDSsound));
	FDSsound.srate = 44100;
	FDSsound.envcps = DivFix(NES_BASECYCLES, 12 * FDSsound.srate, EGCPS_BITS + 5 - 9 + 1);
	FDSsound.envspd = 0xe8 << EGCPS_BITS;
	FDSsound.envdisable = 1;
	FDSsound.phasecps = DivFix(NES_BASECYCLES, 12 * FDSsound.srate, PGCPS_BITS);
	for (i = 0; i < 0x40; i++)
	{
		FDSsound.op[0].wg.wave[i] = (i < 0x20) ? 0x1f : -0x20;
		FDSsound.op[1].wg.wave[i] = 64;
	}
	LogTableInitialize();
	FDSSoundVolume(0);
}

int	FDSsound_Read (int Where)
{
	if (0x4040 <= Where && Where <= 0x407f)
	{
		return FDSsound.op[0].wg.wave[Where & 0x3f] + 0x20;
	}
	if (0x4090 == Where)
		return FDSsound.op[0].eg.volume | 0x40;
	if (0x4092 == Where) /* 4094? */
		return FDSsound.op[1].eg.volume | 0x40;
	return 0;
}

void	FDSsound_Write (int Where, int What)
{
	if (0x4040 <= Where && Where <= 0x407F)
	{
		FDSsound.op[0].wg.wave[Where - 0x4040] = ((int)(What & 0x3f)) - 0x20;
	}
	else if (0x4080 <= Where && Where <= 0x408F)
	{
		struct FDS_OP *pop = &FDSsound.op[(Where & 4) >> 2];
		FDSsound.reg[Where - 0x4080] = What;
		switch (Where & 0xf)
		{
			case 0:
			case 4:
				pop->eg.mode = What & 0xc0;
				if (pop->eg.mode & 0x80)
				{
					pop->eg.volume = (What & 0x3f);
				}
				else
				{
					pop->eg.spd = What & 0x3f;
				}
				break;
			case 5:
				FDSsound.op[1].bias = What & 255;
				break;
			case 2:	case 6:
				pop->pg.freq &= 0x00000F00;
				pop->pg.freq |= (What & 0xFF) << 0;
				pop->pg.spdbase = pop->pg.freq * FDSsound.phasecps;
				break;
			case 3:
				FDSsound.envdisable = What & 0x40;
			case 7:
				pop->pg.freq &= 0x000000FF;
				pop->pg.freq |= (What & 0x0F) << 8;
				pop->pg.spdbase = pop->pg.freq * FDSsound.phasecps;
				pop->wg.disable = What & 0x80;
				if (pop->wg.disable)
				{
					pop->wg.phase = 0;
					pop->wg.wavptr = 0;
					pop->wavebase = 0;
				}
				break;
			case 8:
				if (FDSsound.op[1].wg.disable)
				{
					s32 idx = What & 7;
					if (idx == 4)
					{
						FDSsound.op[1].wavebase = 0;
					}
					FDSsound.op[1].wavebase += wave_delta_table[idx];
					FDSsound.op[1].wg.wave[FDSsound.op[1].wg.wavptr + 0] = (FDSsound.op[1].wavebase + FDSsound.op[1].bias + 64) & 255;
					FDSsound.op[1].wavebase += wave_delta_table[idx];
					FDSsound.op[1].wg.wave[FDSsound.op[1].wg.wavptr + 1] = (FDSsound.op[1].wavebase + FDSsound.op[1].bias + 64) & 255;
					FDSsound.op[1].wg.wavptr = (FDSsound.op[1].wg.wavptr + 2) & 0x3f;
				}
				break;
			case 9:
				FDSsound.lvl = (What & 3);
				FDSsound.op[0].wg.disable2 = What & 0x80;
				break;
			case 10:
				FDSsound.envspd = What << EGCPS_BITS;
				break;
		}
	}
}

int	_MAPINT	FDSsound_Get (int numCycles)
{
	return FDSSoundRender() >> 10;	// current code does not run per-cycle
}

int	_MAPINT	FDSsound_SaveLoad (int mode, int x, char *data)
{
	return x;
}

void	FDSsound_Destroy (void)
{
}
