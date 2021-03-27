/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

/* Modified for usage in VRC7 sound emulation

  Copyright (C) Mitsutaka Okazaki 2004

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from
  the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

*/

/***********************************************************************************

	emu2413.c -- YM2413 emulator written by Mitsutaka Okazaki 2001

	2001 01-08 : Version 0.10 -- 1st version.
	2001 01-15 : Version 0.20 -- semi-public version.
	2001 01-16 : Version 0.30 -- 1st public version.
	2001 01-17 : Version 0.31 -- Fixed bassdrum problem.
		   : Version 0.32 -- LPF implemented.
	2001 01-18 : Version 0.33 -- Fixed the drum problem, refine the mix-down method.
				  -- Fixed the LFO bug.
	2001 01-24 : Version 0.35 -- Fixed the drum problem, 
				     support undocumented EG behavior.
	2001 02-02 : Version 0.38 -- Improved the performance.
				     Fixed the hi-hat and cymbal model.
				     Fixed the default percussive datas.
				     Noise reduction.
				     Fixed the feedback problem.
	2001 03-03 : Version 0.39 -- Fixed some drum bugs.
				     Improved the performance.
	2001 03-04 : Version 0.40 -- Improved the feedback.
				     Change the default table size.
				     Clock and Rate can be changed during play.
	2001 06-24 : Version 0.50 -- Improved the hi-hat and the cymbal tone.
				     Added VRC7 patch (OPLL_reset_patch is changed).
				     Fixed OPLL_reset() bug.
				     Added OPLL_setMask, OPLL_getMask and OPLL_toggleMask.
				     Added OPLL_writeIO.
	2001 09-28 : Version 0.51 -- Removed the noise table.
	2002 01-28 : Version 0.52 -- Added Stereo mode.
	2002 02-07 : Version 0.53 -- Fixed some drum bugs.
	2002 02-20 : Version 0.54 -- Added the best quality mode.
	2002 03-02 : Version 0.55 -- Removed OPLL_init & OPLL_close.
	2002 05-30 : Version 0.60 -- Fixed HH&CYM generator and all voice datas.

	2003 01-24 : Modified by xodnizel to remove code not needed for the VRC7, among other things.

	References: 
		fmopl.c	       -- 1999,2000 written by Tatsuyuki Satoh (MAME development).
		fmopl.c(fixed) -- (C) 2002 Jarek Burczynski.
		s_opl.c	       -- 2001 written by Mamiya (NEZplug development).
		fmgen.cpp      -- 1999,2000 written by cisc.
		fmpac.ill      -- 2000 created by NARUTO.
		MSX-Datapack
		YMU757 data sheet
		YM2143 data sheet

**************************************************************************************/
#include	"..\..\interface.h"
#include	"s_vrc7.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>

#define PI 3.14159265358979323846

namespace VRC7sound
{
enum {OPLL_VRC7_TONE=0};

/* voice data */
typedef struct {
	uint32_t TL,FB,EG,ML,AR,DR,SL,RR,KR,KL,AM,PM,WF;
} OPLL_PATCH;

/* slot */
typedef struct {
	OPLL_PATCH patch;  

	int32_t type;          /* 0 : modulator 1 : carrier */

	/* OUTPUT */
	int32_t feedback;
	int32_t output[2];   /* Output value of slot */

	/* for Phase Generator (PG) */
	uint16_t *sintbl;    /* Wavetable */
	uint32_t phase;      /* Phase */
	uint32_t dphase;     /* Phase increment amount */
	uint32_t pgout;      /* output */

	/* for Envelope Generator (EG) */
	int32_t fnum;          /* F-Number */
	int32_t block;         /* Block */
	int32_t volume;        /* Current volume */
	int32_t sustine;       /* Sustine 1 = ON, 0 = OFF */
	uint32_t tll;	      /* Total Level + Key scale level*/
	uint32_t rks;        /* Key scale offset (Rks) */
	int32_t eg_mode;       /* Current state */
	uint32_t eg_phase;   /* Phase */
	uint32_t eg_dphase;  /* Phase increment amount */
	uint32_t egout;      /* output */

} OPLL_SLOT;

/* Mask */
#define OPLL_MASK_CH(x) (1<<(x))

/* opll */
typedef struct {

	uint32_t adr;
	int32_t out;

#ifndef EMU2413_COMPACTION
	uint32_t realstep;
	uint32_t oplltime;
	uint32_t opllstep;
	int32_t prev, next;
#endif

	/* Register */
	uint8_t LowFreq[6];
	uint8_t HiFreq[6];
	uint8_t InstVol[6];

	uint8_t CustInst[8];

	int32_t slot_on_flag[6 * 2];

	/* Pitch Modulator */
	uint32_t pm_phase;
	int32_t lfo_pm;

	/* Amp Modulator */
	int32_t am_phase;
	int32_t lfo_am;

	uint32_t quality;

	/* Channel Data */
	int32_t patch_number[6];
	int32_t key_status[6];

	/* Slot */
	OPLL_SLOT slot[6 * 2];

	uint32_t mask;

} OPLL;

/* Create Object */
OPLL *OPLL_new(uint32_t clk, uint32_t rate);
void OPLL_delete(OPLL *);

/* Setup */
void OPLL_reset(OPLL *);
void OPLL_set_rate(OPLL *opll, uint32_t r);
void OPLL_set_quality(OPLL *opll, uint32_t q);

/* Port/Register access */
void OPLL_writeIO(OPLL *, uint32_t reg, uint32_t val);
void OPLL_writeReg(OPLL *, uint32_t reg, uint32_t val);

/* Synthsize */
int16_t OPLL_calc(OPLL *);

/* Misc */
void OPLL_forceRefresh(OPLL *);

/* Channel Mask */
uint32_t OPLL_setMask(OPLL *, uint32_t mask);
uint32_t OPLL_toggleMask(OPLL *, uint32_t mask);

// Extracted from VRC7 by Nuke.YKT
static const unsigned char default_inst[15][8] = {
	{0x03,0x21,0x05,0x06,0xE8,0x81,0x42,0x27},
	{0x13,0x41,0x14,0x0D,0xD8,0xF6,0x23,0x12},
	{0x11,0x11,0x08,0x08,0xFA,0xB2,0x20,0x12},
	{0x31,0x61,0x0C,0x07,0xA8,0x64,0x61,0x27},
	{0x32,0x21,0x1E,0x06,0xE1,0x76,0x01,0x28},
	{0x02,0x01,0x06,0x00,0xA3,0xE2,0xF4,0xF4},
	{0x21,0x61,0x1D,0x07,0x82,0x81,0x11,0x07},
	{0x23,0x21,0x22,0x17,0xA2,0x72,0x01,0x17},
	{0x35,0x11,0x25,0x00,0x40,0x73,0x72,0x01},
	{0xB5,0x01,0x0F,0x0F,0xA8,0xA5,0x51,0x02},
	{0x17,0xC1,0x24,0x07,0xF8,0xF8,0x22,0x12},
	{0x71,0x23,0x11,0x06,0x65,0x74,0x18,0x16},
	{0x01,0x02,0xD3,0x05,0xC9,0x95,0x03,0x02},
	{0x61,0x63,0x0C,0x00,0x94,0xC0,0x33,0xF6},
	{0x21,0x72,0x0D,0x00,0xC1,0xD5,0x56,0x06}
};

/* Size of Sintable ( 8 -- 18 can be used. 9 recommended.)*/
#define PG_BITS 9
#define PG_WIDTH (1<<PG_BITS)

/* Phase increment counter */
#define DP_BITS 18
#define DP_WIDTH (1<<DP_BITS)
#define DP_BASE_BITS (DP_BITS - PG_BITS)

/* Dynamic range (Accuracy of sin table) */
#define DB_BITS 8
#define DB_STEP (48.0/(1<<DB_BITS))
#define DB_MUTE (1<<DB_BITS)

/* Dynamic range of envelope */
#define EG_STEP 0.375
#define EG_BITS 7
#define EG_MUTE (1<<EG_BITS)

/* Dynamic range of total level */
#define TL_STEP 0.75
#define TL_BITS 6
#define TL_MUTE (1<<TL_BITS)

/* Dynamic range of sustine level */
#define SL_STEP 3.0
#define SL_BITS 4
#define SL_MUTE (1<<SL_BITS)

#define EG2DB(d) ((d)*(int32_t)(EG_STEP/DB_STEP))
#define TL2EG(d) ((d)*(int32_t)(TL_STEP/EG_STEP))
#define SL2EG(d) ((d)*(int32_t)(SL_STEP/EG_STEP))

#define DB_POS(x) (uint32_t)((x)/DB_STEP)
#define DB_NEG(x) (uint32_t)(DB_MUTE+DB_MUTE+(x)/DB_STEP)

/* Bits for liner value */
#define DB2LIN_AMP_BITS 11
#define SLOT_AMP_BITS (DB2LIN_AMP_BITS)

/* Bits for envelope phase incremental counter */
#define EG_DP_BITS 22
#define EG_DP_WIDTH (1<<EG_DP_BITS)

/* Bits for Pitch and Amp modulator */
#define PM_PG_BITS 8
#define PM_PG_WIDTH (1<<PM_PG_BITS)
#define PM_DP_BITS 16
#define PM_DP_WIDTH (1<<PM_DP_BITS)
#define AM_PG_BITS 8
#define AM_PG_WIDTH (1<<AM_PG_BITS)
#define AM_DP_BITS 16
#define AM_DP_WIDTH (1<<AM_DP_BITS)

/* PM table is calcurated by PM_AMP * pow(2,PM_DEPTH*sin(x)/1200) */
#define PM_AMP_BITS 8
#define PM_AMP (1<<PM_AMP_BITS)

/* PM speed(Hz) and depth(cent) */
#define PM_SPEED 6.4
#define PM_DEPTH 13.75

/* AM speed(Hz) and depth(dB) */
#define AM_SPEED 3.7
#define AM_DEPTH 4.8

/* Cut the lower b bit(s) off. */
#define HIGHBITS(c,b) ((c)>>(b))

/* Leave the lower b bit(s). */
#define LOWBITS(c,b) ((c)&((1<<(b))-1))

/* Expand x which is s bits to d bits. */
#define EXPAND_BITS(x,s,d) ((x)<<((d)-(s)))

/* Expand x which is s bits to d bits and fill expanded bits '1' */
#define EXPAND_BITS_X(x,s,d) (((x)<<((d)-(s)))|((1<<((d)-(s)))-1))

/* Adjust envelope speed which depends on sampling rate. */
#define rate_adjust(x) (rate==49716?x:(uint32_t)((double)(x)*clk/72/rate + 0.5))				/* added 0.5 to round the value*/

#define MOD(o,x) (&(o)->slot[(x)<<1])
#define CAR(o,x) (&(o)->slot[((x)<<1)|1])

#define BIT(s,b) (((s)>>(b))&1)

/* Input clock */
static uint32_t clk = 844451141;
/* Sampling rate */
static uint32_t rate = 3354932;

/* WaveTable for each envelope amp */
static uint16_t fullsintable[PG_WIDTH];
static uint16_t halfsintable[PG_WIDTH];

static uint16_t *waveform[2] = { fullsintable, halfsintable };

/* LFO Table */
static int32_t pmtable[PM_PG_WIDTH];
static int32_t amtable[AM_PG_WIDTH];

/* Phase delta for LFO */
static uint32_t pm_dphase;
static uint32_t am_dphase;

/* dB to Liner table */
static int16_t DB2LIN_TABLE[(DB_MUTE + DB_MUTE) * 2];

/* Liner to Log curve conversion table (for Attack rate). */
static uint16_t AR_ADJUST_TABLE[1 << EG_BITS];

/* Definition of envelope mode */
enum
{ SETTLE, ATTACK, DECAY, SUSHOLD, SUSTINE, RELEASE, FINISH };

/* Phase incr table for Attack */
static uint32_t dphaseARTable[16][16];
/* Phase incr table for Decay and Release */
static uint32_t dphaseDRTable[16][16];

/* KSL + TL Table */
static uint32_t tllTable[16][8][1 << TL_BITS][4];
static int32_t rksTable[2][8][2];

/* Phase incr table for PG */
static uint32_t dphaseTable[512][8][16];

/***************************************************
 
		Create tables
 
****************************************************/
__inline static int32_t
Min (int32_t i, int32_t j)
{
	if (i < j)
		return i;
	else
		return j;
}

/* Table for AR to LogCurve. */
static void
makeAdjustTable (void)
{
	int32_t i;

	AR_ADJUST_TABLE[0] = (1 << EG_BITS);
	for (i = 1; i < 128; i++)
		AR_ADJUST_TABLE[i] = (uint16_t) ((double) (1 << EG_BITS) - 1 - (1 << EG_BITS) * log ((double)i) / log (128.0));
}


/* Table for dB(0 -- (1<<DB_BITS)-1) to Liner(0 -- DB2LIN_AMP_WIDTH) */
static void
makeDB2LinTable (void)
{
	int32_t i;

	for (i = 0; i < DB_MUTE + DB_MUTE; i++)
	{
		DB2LIN_TABLE[i] = (int16_t) ((double) ((1 << DB2LIN_AMP_BITS) - 1) * pow (10.0, -(double) i * DB_STEP / 20));
		if (i >= DB_MUTE) DB2LIN_TABLE[i] = 0;
		DB2LIN_TABLE[i + DB_MUTE + DB_MUTE] = (int16_t) (-DB2LIN_TABLE[i]);
	}
}

/* Liner(+0.0 - +1.0) to dB((1<<DB_BITS) - 1 -- 0) */
static int32_t
lin2db (double d)
{
	if (d == 0)
		return (DB_MUTE - 1);
	else
		return Min (-(int32_t) (20.0 * log10 (d) / DB_STEP), DB_MUTE-1);	/* 0 -- 127 */
}


/* Sin Table */
static void
makeSinTable (void)
{
	int32_t i;

	for (i = 0; i < PG_WIDTH / 4; i++)
	{
		fullsintable[i] = (uint16_t) lin2db (sin (2.0 * PI * i / PG_WIDTH) );
	}

	for (i = 0; i < PG_WIDTH / 4; i++)
	{
		fullsintable[PG_WIDTH / 2 - 1 - i] = fullsintable[i];
	}

	for (i = 0; i < PG_WIDTH / 2; i++)
	{
		fullsintable[PG_WIDTH / 2 + i] = (uint16_t) (DB_MUTE + DB_MUTE + fullsintable[i]);
	}

	for (i = 0; i < PG_WIDTH / 2; i++)
		halfsintable[i] = fullsintable[i];
	for (i = PG_WIDTH / 2; i < PG_WIDTH; i++)
		halfsintable[i] = fullsintable[0];
}

/* Table for Pitch Modulator */
static void
makePmTable (void)
{
	int32_t i;

	for (i = 0; i < PM_PG_WIDTH; i++)
		pmtable[i] = (int32_t) ((double) PM_AMP * pow (2.0, (double) PM_DEPTH * sin (2.0 * PI * i / PM_PG_WIDTH) / 1200));
}

/* Table for Amp Modulator */
static void
makeAmTable (void)
{
	int32_t i;

	for (i = 0; i < AM_PG_WIDTH; i++)
		amtable[i] = (int32_t) ((double) AM_DEPTH / 2 / DB_STEP * (1.0 + sin (2.0 * PI * i / PM_PG_WIDTH)));
}

/* Phase increment counter table */
static void
makeDphaseTable (void)
{
	uint32_t fnum, block, ML;
	uint32_t mltable[16] =
		{ 1, 1 * 2, 2 * 2, 3 * 2, 4 * 2, 5 * 2, 6 * 2, 7 * 2, 8 * 2, 9 * 2, 10 * 2, 10 * 2, 12 * 2, 12 * 2, 15 * 2, 15 * 2 };

	for (fnum = 0; fnum < 512; fnum++)
		for (block = 0; block < 8; block++)
			for (ML = 0; ML < 16; ML++)
				dphaseTable[fnum][block][ML] = rate_adjust (((fnum * mltable[ML]) << block) >> (20 - DP_BITS));
}

static void
makeTllTable (void)
{
#define dB2(x) ((x)*2)

	static double kltable[16] = {
		dB2 (0.000), dB2 (9.000), dB2 (12.000), dB2 (13.875), dB2 (15.000), dB2 (16.125), dB2 (16.875), dB2 (17.625),
		dB2 (18.000), dB2 (18.750), dB2 (19.125), dB2 (19.500), dB2 (19.875), dB2 (20.250), dB2 (20.625), dB2 (21.000)
	};

	int32_t tmp;
	int32_t fnum, block, TL, KL;

	for (fnum = 0; fnum < 16; fnum++)
		for (block = 0; block < 8; block++)
			for (TL = 0; TL < 64; TL++)
				for (KL = 0; KL < 4; KL++)
				{
					if (KL == 0)
					{
						tllTable[fnum][block][TL][KL] = TL2EG (TL);
					}
					else
					{
						tmp = (int32_t) (kltable[fnum] - dB2 (3.000) * (7 - block));
						if (tmp <= 0)
							tllTable[fnum][block][TL][KL] = TL2EG (TL);
						else
							tllTable[fnum][block][TL][KL] = (uint32_t) ((tmp >> (3 - KL)) / EG_STEP) + TL2EG (TL);
					}
				}
}

#ifdef USE_SPEC_ENV_SPEED
static double attacktime[16][4] = {
	{0, 0, 0, 0},
	{1730.15, 1400.60, 1153.43, 988.66},
	{865.08, 700.30, 576.72, 494.33},
	{432.54, 350.15, 288.36, 247.16},
	{216.27, 175.07, 144.18, 123.58},
	{108.13, 87.54, 72.09, 61.79},
	{54.07, 43.77, 36.04, 30.90},
	{27.03, 21.88, 18.02, 15.45},
	{13.52, 10.94, 9.01, 7.72},
	{6.76, 5.47, 4.51, 3.86},
	{3.38, 2.74, 2.25, 1.93},
	{1.69, 1.37, 1.13, 0.97},
	{0.84, 0.70, 0.60, 0.54},
	{0.50, 0.42, 0.34, 0.30},
	{0.28, 0.22, 0.18, 0.14},
	{0.00, 0.00, 0.00, 0.00}
};

static double decaytime[16][4] = {
	{0, 0, 0, 0},
	{20926.60, 16807.20, 14006.00, 12028.60},
	{10463.30, 8403.58, 7002.98, 6014.32},
	{5231.64, 4201.79, 3501.49, 3007.16},
	{2615.82, 2100.89, 1750.75, 1503.58},
	{1307.91, 1050.45, 875.37, 751.79},
	{653.95, 525.22, 437.69, 375.90},
	{326.98, 262.61, 218.84, 187.95},
	{163.49, 131.31, 109.42, 93.97},
	{81.74, 65.65, 54.71, 46.99},
	{40.87, 32.83, 27.36, 23.49},
	{20.44, 16.41, 13.68, 11.75},
	{10.22, 8.21, 6.84, 5.87},
	{5.11, 4.10, 3.42, 2.94},
	{2.55, 2.05, 1.71, 1.47},
	{1.27, 1.27, 1.27, 1.27}
};
#endif

/* Rate Table for Attack */
static void
makeDphaseARTable (void)
{
	int32_t AR, Rks, RM, RL;
#ifdef USE_SPEC_ENV_SPEED
	uint32_t attacktable[16][4];

	for (RM = 0; RM < 16; RM++)
		for (RL = 0; RL < 4; RL++)
		{
			if (RM == 0)
				attacktable[RM][RL] = 0;
			else if (RM == 15)
				attacktable[RM][RL] = EG_DP_WIDTH;
			else
				attacktable[RM][RL] = (uint32_t) ((double) (1 << EG_DP_BITS) / (attacktime[RM][RL] * 3579545 / 72000));

		}
#endif

	for (AR = 0; AR < 16; AR++)
		for (Rks = 0; Rks < 16; Rks++)
		{
			RM = AR + (Rks >> 2);
			RL = Rks & 3;
			if (RM > 15)
				RM = 15;
			switch (AR)
			{
			case 0:
				dphaseARTable[AR][Rks] = 0;
				break;
			case 15:
				dphaseARTable[AR][Rks] = 0;/*EG_DP_WIDTH;*/ 
				break;
			default:
#ifdef USE_SPEC_ENV_SPEED
				dphaseARTable[AR][Rks] = rate_adjust (attacktable[RM][RL]);
#else
				dphaseARTable[AR][Rks] = rate_adjust ((3 * (RL + 4) << (RM + 1)));
#endif
				break;
			}
		}
}

/* Rate Table for Decay and Release */
static void
makeDphaseDRTable (void)
{
	int32_t DR, Rks, RM, RL;

#ifdef USE_SPEC_ENV_SPEED
	uint32_t decaytable[16][4];

	for (RM = 0; RM < 16; RM++)
		for (RL = 0; RL < 4; RL++)
			if (RM == 0)
				decaytable[RM][RL] = 0;
			else
				decaytable[RM][RL] = (uint32_t) ((double) (1 << EG_DP_BITS) / (decaytime[RM][RL] * 3579545 / 72000));
#endif

	for (DR = 0; DR < 16; DR++)
		for (Rks = 0; Rks < 16; Rks++)
		{
			RM = DR + (Rks >> 2);
			RL = Rks & 3;
			if (RM > 15)
				RM = 15;
			switch (DR)
			{
			case 0:
				dphaseDRTable[DR][Rks] = 0;
				break;
			default:
#ifdef USE_SPEC_ENV_SPEED
				dphaseDRTable[DR][Rks] = rate_adjust (decaytable[RM][RL]);
#else
				dphaseDRTable[DR][Rks] = rate_adjust ((RL + 4) << (RM - 1));
#endif
				break;
			}
		}
}

static void
makeRksTable (void)
{

	int32_t fnum8, block, KR;

	for (fnum8 = 0; fnum8 < 2; fnum8++)
		for (block = 0; block < 8; block++)
			for (KR = 0; KR < 2; KR++)
			{
				if (KR != 0)
					rksTable[fnum8][block][KR] = (block << 1) + fnum8;
				else
					rksTable[fnum8][block][KR] = block >> 1;
			}
}

/************************************************************

			Calc Parameters

************************************************************/

__inline static uint32_t
calc_eg_dphase (OPLL_SLOT * slot)
{

	switch (slot->eg_mode)
	{
	case ATTACK:
		return dphaseARTable[slot->patch.AR][slot->rks];

	case DECAY:
		return dphaseDRTable[slot->patch.DR][slot->rks];

	case SUSHOLD:
		return 0;

	case SUSTINE:
		return dphaseDRTable[slot->patch.RR][slot->rks];

	case RELEASE:
		if (slot->sustine)
			return dphaseDRTable[5][slot->rks];
		else if (slot->patch.EG)
			return dphaseDRTable[slot->patch.RR][slot->rks];
		else
			return dphaseDRTable[7][slot->rks];

	case FINISH:
		return 0;

	default:
		return 0;
	}
}

/*************************************************************

		OPLL internal interfaces

*************************************************************/

#define UPDATE_PG(S)	(S)->dphase = dphaseTable[(S)->fnum][(S)->block][(S)->patch.ML]
#define UPDATE_TLL(S)\
(((S)->type==0)?\
((S)->tll = tllTable[((S)->fnum)>>5][(S)->block][(S)->patch.TL][(S)->patch.KL]):\
((S)->tll = tllTable[((S)->fnum)>>5][(S)->block][(S)->volume][(S)->patch.KL]))
#define UPDATE_RKS(S) (S)->rks = rksTable[((S)->fnum)>>8][(S)->block][(S)->patch.KR]
#define UPDATE_WF(S)	(S)->sintbl = waveform[(S)->patch.WF]
#define UPDATE_EG(S)	(S)->eg_dphase = calc_eg_dphase(S)
#define UPDATE_ALL(S)\
	UPDATE_PG(S);\
	UPDATE_TLL(S);\
	UPDATE_RKS(S);\
	UPDATE_WF(S); \
	UPDATE_EG(S)		/* EG should be updated last. */


/* Slot key on	*/
__inline static void
slotOn (OPLL_SLOT * slot)
{
	slot->eg_mode = ATTACK;
	slot->eg_phase = 0;
	slot->phase = 0;
}

/* Slot key on without reseting the phase */
__inline static void
slotOn2 (OPLL_SLOT * slot)
{
	slot->eg_mode = ATTACK;
	slot->eg_phase = 0;
}

/* Slot key off */
__inline static void
slotOff (OPLL_SLOT * slot)
{
	if (slot->eg_mode == ATTACK)
		slot->eg_phase = EXPAND_BITS (AR_ADJUST_TABLE[HIGHBITS (slot->eg_phase, EG_DP_BITS - EG_BITS)], EG_BITS, EG_DP_BITS);
	slot->eg_mode = RELEASE;
}

/* Channel key on */
__inline static void
keyOn (OPLL * opll, int32_t i)
{
	if (!opll->slot_on_flag[i * 2])
		slotOn (MOD(opll,i));
	if (!opll->slot_on_flag[i * 2 + 1])
		slotOn (CAR(opll,i));
	opll->key_status[i] = 1;
}

/* Channel key off */
__inline static void
keyOff (OPLL * opll, int32_t i)
{
	if (opll->slot_on_flag[i * 2 + 1])
		slotOff (CAR(opll,i));
	opll->key_status[i] = 0;
}

/* Set sustine parameter */
__inline static void
setSustine (OPLL * opll, int32_t c, int32_t sustine)
{
	CAR(opll,c)->sustine = sustine;
	if (MOD(opll,c)->type)
		MOD(opll,c)->sustine = sustine;
}

/* Volume : 6bit ( Volume register << 2 ) */
__inline static void
setVolume (OPLL * opll, int32_t c, int32_t volume)
{
	CAR(opll,c)->volume = volume;
}

__inline static void
setSlotVolume (OPLL_SLOT * slot, int32_t volume)
{
	slot->volume = volume;
}

/* Set F-Number ( fnum : 9bit ) */
__inline static void
setFnumber (OPLL * opll, int32_t c, int32_t fnum)
{
	CAR(opll,c)->fnum = fnum;
	MOD(opll,c)->fnum = fnum;
}

/* Set Block data (block : 3bit ) */
__inline static void
setBlock (OPLL * opll, int32_t c, int32_t block)
{
	CAR(opll,c)->block = block;
	MOD(opll,c)->block = block;
}

__inline static void update_key_status (OPLL * opll)
{
	int ch;

	for (ch = 0; ch < 6; ch++)
		opll->slot_on_flag[ch * 2] = opll->slot_on_flag[ch * 2 + 1] = (opll->HiFreq[ch]) & 0x10;
}

/***********************************************************

											Initializing

***********************************************************/

static void
OPLL_SLOT_reset (OPLL_SLOT * slot, int type)
{
	slot->type = type;
	slot->sintbl = waveform[0];
	slot->phase = 0;
	slot->dphase = 0;
	slot->output[0] = 0;
	slot->output[1] = 0;
	slot->feedback = 0;
	slot->eg_mode = SETTLE;
	slot->eg_phase = EG_DP_WIDTH;
	slot->eg_dphase = 0;
	slot->rks = 0;
	slot->tll = 0;
	slot->sustine = 0;
	slot->fnum = 0;
	slot->block = 0;
	slot->volume = 0;
	slot->pgout = 0;
	slot->egout = 0;
}

static void
internal_refresh (void)
{
	makeDphaseTable ();
	makeDphaseARTable ();
	makeDphaseDRTable ();
	pm_dphase = (uint32_t) rate_adjust (PM_SPEED * PM_DP_WIDTH / (clk / 72));
	am_dphase = (uint32_t) rate_adjust (AM_SPEED * AM_DP_WIDTH / (clk / 72));
}

static void
maketables (uint32_t c, uint32_t r)
{
	if (c != clk)
	{
		clk = c;
		makePmTable ();
		makeAmTable ();
		makeDB2LinTable ();
		makeAdjustTable ();
		makeTllTable ();
		makeRksTable ();
		makeSinTable ();
		//makeDefaultPatch ();
	}

	if (r != rate)
	{
		rate = r;
		internal_refresh ();
	}
}

OPLL *OPLL_new (uint32_t _clk, uint32_t _rate)
{
	OPLL *opll;

	maketables (_clk, _rate);

	opll = (OPLL *) calloc (sizeof (OPLL), 1);
	if (opll == NULL)
		return NULL;

	opll->mask = 0;

	OPLL_reset (opll);

	return opll;
}


void
OPLL_delete (OPLL * opll)
{
	free (opll);
}

/* Reset whole of OPLL except patch datas. */
void
OPLL_reset (OPLL * opll)
{
	int32_t i;

	if (!opll)
		return;

	opll->adr = 0;
	opll->out = 0;

	opll->pm_phase = 0;
	opll->am_phase = 0;

	opll->mask = 0;

	for (i = 0; i < 12; i++)
		OPLL_SLOT_reset(&opll->slot[i], i%2);

	for (i = 0; i < 6; i++)
	{
		opll->key_status[i] = 0;
		//setPatch (opll, i, 0);
	}

	for (i = 0; i < 0x40; i++)
		OPLL_writeReg (opll, i, 0);

#ifndef EMU2413_COMPACTION
	opll->realstep = (uint32_t) ((1 << 31) / rate);
	opll->opllstep = (uint32_t) ((1 << 31) / (clk / 72));
	opll->oplltime = 0;
#endif
}

/* Force Refresh (When external program changes some parameters). */
void
OPLL_forceRefresh (OPLL * opll)
{
	int32_t i;

	if (opll == NULL)
		return;

	for (i = 0; i < 12; i++)
	{
		UPDATE_PG (&opll->slot[i]);
		UPDATE_RKS (&opll->slot[i]);
		UPDATE_TLL (&opll->slot[i]);
		UPDATE_WF (&opll->slot[i]);
		UPDATE_EG (&opll->slot[i]);
	}
}

void
OPLL_set_rate (OPLL * opll, uint32_t r)
{
	if (opll->quality)
		rate = 49716;
	else
		rate = r;
	internal_refresh ();
	rate = r;
}

void
OPLL_set_quality (OPLL * opll, uint32_t q)
{
	opll->quality = q;
	OPLL_set_rate (opll, rate);
}

/*********************************************************

								 Generate wave data

*********************************************************/
/* Convert Amp(0 to EG_HEIGHT) to Phase(0 to 2PI). */
#if ( SLOT_AMP_BITS - PG_BITS ) > 0
#define wave2_2pi(e)	( (e) >> ( SLOT_AMP_BITS - PG_BITS ))
#else
#define wave2_2pi(e)	( (e) << ( PG_BITS - SLOT_AMP_BITS ))
#endif

/* Convert Amp(0 to EG_HEIGHT) to Phase(0 to 4PI). */
#if ( SLOT_AMP_BITS - PG_BITS - 1 ) == 0
#define wave2_4pi(e)	(e)
#elif ( SLOT_AMP_BITS - PG_BITS - 1 ) > 0
#define wave2_4pi(e)	( (e) >> ( SLOT_AMP_BITS - PG_BITS - 1 ))
#else
#define wave2_4pi(e)	( (e) << ( 1 + PG_BITS - SLOT_AMP_BITS ))
#endif

/* Convert Amp(0 to EG_HEIGHT) to Phase(0 to 8PI). */
#if ( SLOT_AMP_BITS - PG_BITS - 2 ) == 0
#define wave2_8pi(e)	(e)
#elif ( SLOT_AMP_BITS - PG_BITS - 2 ) > 0
#define wave2_8pi(e)	( (e) >> ( SLOT_AMP_BITS - PG_BITS - 2 ))
#else
#define wave2_8pi(e)	( (e) << ( 2 + PG_BITS - SLOT_AMP_BITS ))
#endif



/* Update AM, PM unit */
static void
update_ampm (OPLL * opll)
{
	opll->pm_phase = (opll->pm_phase + pm_dphase) & (PM_DP_WIDTH - 1);
	opll->am_phase = (opll->am_phase + am_dphase) & (AM_DP_WIDTH - 1);
	opll->lfo_am = amtable[HIGHBITS (opll->am_phase, AM_DP_BITS - AM_PG_BITS)];
	opll->lfo_pm = pmtable[HIGHBITS (opll->pm_phase, PM_DP_BITS - PM_PG_BITS)];
}

/* PG */
__inline static void
calc_phase (OPLL_SLOT * slot, int32_t lfo)
{
	if (slot->patch.PM)
		slot->phase += (slot->dphase * lfo) >> PM_AMP_BITS;
	else
		slot->phase += slot->dphase;

	slot->phase &= (DP_WIDTH - 1);

	slot->pgout = HIGHBITS (slot->phase, DP_BASE_BITS);
}

/* EG */
static void
calc_envelope (OPLL_SLOT * slot, int32_t lfo)
{
#define S2E(x) (SL2EG((int32_t)(x/SL_STEP))<<(EG_DP_BITS-EG_BITS))

	static uint32_t SL[16] = {
		S2E (0.0), S2E (3.0), S2E (6.0), S2E (9.0), S2E (12.0), S2E (15.0), S2E (18.0), S2E (21.0),
		S2E (24.0), S2E (27.0), S2E (30.0), S2E (33.0), S2E (36.0), S2E (39.0), S2E (42.0), S2E (48.0)
	};

	uint32_t egout;

	switch (slot->eg_mode)
	{

	case ATTACK:
		egout = AR_ADJUST_TABLE[HIGHBITS (slot->eg_phase, EG_DP_BITS - EG_BITS)];
		slot->eg_phase += slot->eg_dphase;
		if((EG_DP_WIDTH & slot->eg_phase)||(slot->patch.AR==15))
		{
			egout = 0;
			slot->eg_phase = 0;
			slot->eg_mode = DECAY;
			UPDATE_EG (slot);
		}
		break;

	case DECAY:
		egout = HIGHBITS (slot->eg_phase, EG_DP_BITS - EG_BITS);
		slot->eg_phase += slot->eg_dphase;
		if (slot->eg_phase >= SL[slot->patch.SL])
		{
			if (slot->patch.EG)
			{
				slot->eg_phase = SL[slot->patch.SL];
				slot->eg_mode = SUSHOLD;
				UPDATE_EG (slot);
			}
			else
			{
				slot->eg_phase = SL[slot->patch.SL];
				slot->eg_mode = SUSTINE;
				UPDATE_EG (slot);
			}
		}
		break;

	case SUSHOLD:
		egout = HIGHBITS (slot->eg_phase, EG_DP_BITS - EG_BITS);
		if (slot->patch.EG == 0)
		{
			slot->eg_mode = SUSTINE;
			UPDATE_EG (slot);
		}
		break;

	case SUSTINE:
	case RELEASE:
		egout = HIGHBITS (slot->eg_phase, EG_DP_BITS - EG_BITS);
		slot->eg_phase += slot->eg_dphase;
		if (egout >= (1 << EG_BITS))
		{
			slot->eg_mode = FINISH;
			egout = (1 << EG_BITS) - 1;
		}
		break;

	case FINISH:
		egout = (1 << EG_BITS) - 1;
		break;

	default:
		egout = (1 << EG_BITS) - 1;
		break;
	}

	if (slot->patch.AM)
		egout = EG2DB (egout + slot->tll) + lfo;
	else
		egout = EG2DB (egout + slot->tll);

	if (egout >= DB_MUTE)
		egout = DB_MUTE - 1;
	
	slot->egout = egout;
}

/* CARRIOR */
__inline static int32_t
calc_slot_car (OPLL_SLOT * slot, int32_t fm)
{
	slot->output[1] = slot->output[0];

	if (slot->egout >= (DB_MUTE - 1))
	{
		slot->output[0] = 0;
	}
	else
	{
		slot->output[0] = DB2LIN_TABLE[slot->sintbl[(slot->pgout+wave2_8pi(fm))&(PG_WIDTH-1)] + slot->egout];
	}

	return (slot->output[1] + slot->output[0]) >> 1;
}

/* MODULATOR */
__inline static int32_t
calc_slot_mod (OPLL_SLOT * slot)
{
	int32_t fm;

	slot->output[1] = slot->output[0];

	if (slot->egout >= (DB_MUTE - 1))
	{
		slot->output[0] = 0;
	}
	else if (slot->patch.FB != 0)
	{
		fm = wave2_4pi (slot->feedback) >> (7 - slot->patch.FB);
		slot->output[0] = DB2LIN_TABLE[slot->sintbl[(slot->pgout + fm)&(PG_WIDTH-1)] + slot->egout];
	}
	else
	{
		slot->output[0] = DB2LIN_TABLE[slot->sintbl[slot->pgout] + slot->egout];
	}

	slot->feedback = (slot->output[1] + slot->output[0]) >> 1;

	return slot->feedback;

}

static __inline int16_t calc (OPLL * opll)
{
	int32_t inst = 0, out = 0;
	int32_t i;

	update_ampm (opll);

	for (i = 0; i < 12; i++)
	{
		calc_phase(&opll->slot[i],opll->lfo_pm);
		calc_envelope(&opll->slot[i],opll->lfo_am);
	}

	for (i = 0; i < 6; i++)
		if (!(opll->mask & OPLL_MASK_CH (i)) && (CAR(opll,i)->eg_mode != FINISH))
			inst += calc_slot_car (CAR(opll,i), calc_slot_mod(MOD(opll,i)));

	out = inst;
	return (int16_t) out;
}

#ifdef EMU2413_COMPACTION
int16_t
OPLL_calc (OPLL * opll)
{
	return calc (opll);
}
#else
int16_t
OPLL_calc (OPLL * opll)
{
	if (!opll->quality)	
	 return calc (opll);

	while (opll->realstep > opll->oplltime)
	{
		opll->oplltime += opll->opllstep;
		opll->prev = opll->next;
		opll->next = calc (opll);
	}

	opll->oplltime -= opll->realstep;
	opll->out = (int16_t) (((double) opll->next * (opll->opllstep - opll->oplltime)
			+ (double) opll->prev * opll->oplltime) / opll->opllstep);

	return (int16_t) opll->out;
}
#endif

uint32_t
OPLL_setMask (OPLL * opll, uint32_t mask)
{
	uint32_t ret;

	if (opll)
	{
		ret = opll->mask;
		opll->mask = mask;
		return ret;
	}
	else
		return 0;
}

uint32_t
OPLL_toggleMask (OPLL * opll, uint32_t mask)
{
	uint32_t ret;

	if (opll)
	{
		ret = opll->mask;
		opll->mask ^= mask;
		return ret;
	}
	else
		return 0;
}

/****************************************************

											 I/O Ctrl

*****************************************************/

static void setInstrument(OPLL * opll, unsigned int i, unsigned int inst)
{
	const uint8_t *src;
	OPLL_PATCH *modp, *carp;

	opll->patch_number[i]=inst;

	if(inst)
		src=default_inst[inst-1];
	else
		src=opll->CustInst;

	modp=&MOD(opll,i)->patch;
	carp=&CAR(opll,i)->patch;

	modp->AM=(src[0]>>7)&1;
	modp->PM=(src[0]>>6)&1;
	modp->EG=(src[0]>>5)&1;
	modp->KR=(src[0]>>4)&1;
	modp->ML=(src[0]&0xF);

	carp->AM=(src[1]>>7)&1;
	carp->PM=(src[1]>>6)&1;
	carp->EG=(src[1]>>5)&1;
	carp->KR=(src[1]>>4)&1;
	carp->ML=(src[1]&0xF);	

	modp->KL=(src[2]>>6)&3;
	modp->TL=(src[2]&0x3F);

	carp->KL = (src[3] >> 6) & 3; 
	carp->WF = (src[3] >> 4) & 1;

	modp->WF = (src[3] >> 3) & 1;

	modp->FB = (src[3]) & 7;

	modp->AR = (src[4]>>4)&0xF;
	modp->DR = (src[4]&0xF);

	carp->AR = (src[5]>>4)&0xF;
	carp->DR = (src[5]&0xF);

	modp->SL = (src[6]>>4)&0xF;
	modp->RR = (src[6]&0xF);
 
	carp->SL = (src[7]>>4)&0xF;
	carp->RR = (src[7]&0xF);
}


void
OPLL_writeReg (OPLL * opll, uint32_t reg, uint32_t data)
{

	int32_t i, v, ch;

	data = data & 0xff;
	reg = reg & 0x3f;

	switch (reg)
	{
	case 0x00:
		opll->CustInst[0]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_PG (MOD(opll,i));
				UPDATE_RKS (MOD(opll,i));
				UPDATE_EG (MOD(opll,i));
			}
		}
		break;

	case 0x01:
		opll->CustInst[1]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_PG (CAR(opll,i));
				UPDATE_RKS (CAR(opll,i));
				UPDATE_EG (CAR(opll,i));
			}
		}
		break;

	case 0x02:
		opll->CustInst[2]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_TLL(MOD(opll,i));
			}
		}
		break;

	case 0x03:
		opll->CustInst[3]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_WF(MOD(opll,i));
				UPDATE_WF(CAR(opll,i));
			}
		}
		break;

	case 0x04:
		opll->CustInst[4]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_EG (MOD(opll,i));
			}
		}
		break;

	case 0x05:
		opll->CustInst[5]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_EG(CAR(opll,i));
			}
		}
		break;

	case 0x06:
		opll->CustInst[6]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_EG (MOD(opll,i));
			}
		}
		break;

	case 0x07:
		opll->CustInst[7]=(uint8_t)data;
		for (i = 0; i < 6; i++)
		{
			if (opll->patch_number[i] == 0)
			{
				setInstrument(opll, i, 0);
				UPDATE_EG (CAR(opll,i));
			}
		}
		break;

	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
		ch = reg - 0x10;
		opll->LowFreq[ch]=(uint8_t)data;
		setFnumber (opll, ch, data + ((opll->HiFreq[ch] & 1) << 8));
		UPDATE_ALL (MOD(opll,ch));
		UPDATE_ALL (CAR(opll,ch));
		break;

	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:
		ch = reg - 0x20;
		opll->HiFreq[ch]=(uint8_t)data;

		setFnumber (opll, ch, ((data & 1) << 8) + opll->LowFreq[ch]);
		setBlock (opll, ch, (data >> 1) & 7);
		setSustine (opll, ch, (data >> 5) & 1);
		if (data & 0x10)
			keyOn (opll, ch);
		else
			keyOff (opll, ch);
		UPDATE_ALL (MOD(opll,ch));
		UPDATE_ALL (CAR(opll,ch));
		update_key_status (opll);
		break;

	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
		opll->InstVol[reg-0x30]=(uint8_t)data;
		i = (data >> 4) & 15;
		v = data & 15;
		setInstrument(opll, reg-0x30, i);
		setVolume (opll, reg - 0x30, v << 2);
		UPDATE_ALL (MOD(opll,reg - 0x30));
		UPDATE_ALL (CAR(opll,reg - 0x30));
		break;

	default:
		break;

	}
}

// Konami VRC7, based on the YM2413

OPLL *OPL = NULL;
uint8_t regAddr;

void	Load (void)
{
	if (OPL != NULL)
	{
		MessageBox(hWnd, _T("YM2413 already created!"), _T("VRC7"), MSGBOX_FLAGS);
		return;
	}
	else
	{
		OPL = OPLL_new(3579545, 44100);
		if (OPL == NULL)
		{
			MessageBox(hWnd, _T("Unable to create YM2413!"), _T("VRC7"), MSGBOX_FLAGS);
			return;
		}
	}
}

void	Reset (RESET_TYPE ResetType)
{
}

void	Unload (void)
{
	if (OPL)
	{
		OPLL_delete(OPL);
		OPL = NULL;
	}
}

void	Write (int Addr, int Val)
{
	switch (Addr & 0xF030)
	{
	case 0x9010:	regAddr = Val;				break;
	case 0x9030:	OPLL_writeReg(OPL, regAddr, Val);	break;
	}
}

int	MAPINT	Get (int numCycles)
{
	return OPLL_calc(OPL) << 3;	// currently don't use numCycles
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	int i;
	SAVELOAD_LONG(mode,offset,data,OPL->adr);
	SAVELOAD_LONG(mode,offset,data,OPL->out);
	SAVELOAD_LONG(mode,offset,data,OPL->realstep);
	SAVELOAD_LONG(mode,offset,data,OPL->oplltime);
	SAVELOAD_LONG(mode,offset,data,OPL->opllstep);
	SAVELOAD_LONG(mode,offset,data,OPL->prev);
	SAVELOAD_LONG(mode,offset,data,OPL->next);
	for (i = 0; i < 6; i++)
	{
		SAVELOAD_BYTE(mode,offset,data,OPL->LowFreq[i]);
		SAVELOAD_BYTE(mode,offset,data,OPL->HiFreq[i]);
		SAVELOAD_BYTE(mode,offset,data,OPL->InstVol[i]);
	}
	for (i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode,offset,data,OPL->CustInst[i]);
	for (i = 0; i < 12; i++)
		SAVELOAD_LONG(mode,offset,data,OPL->slot_on_flag[i]);
	SAVELOAD_LONG(mode,offset,data,OPL->pm_phase);
	SAVELOAD_LONG(mode,offset,data,OPL->lfo_pm);
	SAVELOAD_LONG(mode,offset,data,OPL->am_phase);
	SAVELOAD_LONG(mode,offset,data,OPL->lfo_am);
	SAVELOAD_LONG(mode,offset,data,OPL->quality);
	for (i = 0; i < 6; i++)
	{
		SAVELOAD_LONG(mode,offset,data,OPL->patch_number[i]);
		SAVELOAD_LONG(mode,offset,data,OPL->key_status[i]);
	}
	for (i = 0; i < 12; i++)
	{
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.TL);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.FB);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.EG);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.ML);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.AR);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.DR);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.SL);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.RR);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.KR);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.KL);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.AM);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.PM);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].patch.WF);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].type);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].feedback);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].output[0]);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].output[1]);
		// commented fields are recalculated
//		SAVELOAD_WORD(mode,offset,data,OPL->slot[i].sintbl[0]);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].phase);
//		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].dphase);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].pgout);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].fnum);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].block);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].volume);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].sustine);
//		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].tll);
//		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].rks);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].eg_mode);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].eg_phase);
//		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].eg_dphase);
		SAVELOAD_LONG(mode,offset,data,OPL->slot[i].egout);
	}

	SAVELOAD_LONG(mode,offset,data,OPL->mask);

	if (IsLoad(mode))
	{
		for (i = 0; i < 6; i++)
		{
			UPDATE_ALL(MOD(OPL, i));
			UPDATE_ALL(CAR(OPL, i));
		}
	}
	SAVELOAD_BYTE(mode, offset, data, regAddr);
	return offset;
}
} // namespace VRC7sound
