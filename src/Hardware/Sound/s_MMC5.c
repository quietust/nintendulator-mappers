#include	"..\..\interface.h"
#include	"s_MMC5.h"

// Nintendo MMC5, based on the NES APU

typedef	struct	MMC5Sqr
{

	union
	{
		struct
		{
			unsigned volume   : 4;
			unsigned envelope : 1;
			unsigned wavehold : 1;
			unsigned duty     : 2;
			unsigned freq     :11;
			unsigned timer    : 5;
		};
		struct
		{
			unsigned byte0 : 8;
			unsigned byte2 : 8;
			unsigned byte3 : 8;
		};
	};
	unsigned char Vol, Envelope, CurD;
	int Timer, EnvCtr;
	BOOL Enabled, ValidFreq, Active;
	signed long Cycles;
	signed long Pos;
}	TMMC5sqr, *PMMC5sqr;

typedef	struct	MMC5sound
{
	TMMC5sqr Sqr0, Sqr1;
	int PCM;
}	TMMC5sound, *PMMC5sound;

static	TMMC5sound	MMC5sound;

#define	QUARTER_FRAME_LEN	7457

static	const	unsigned char	LengthCounts[32] = {0x0A,0xFE,0x14,0x02,0x28,0x04,0x50,0x06,0xA0,0x08,0x3C,0x0A,0x0E,0x0C,0x1A,0x0E,0x0C,0x10,0x18,0x12,0x30,0x14,0x60,0x16,0xC0,0x18,0x48,0x1A,0x10,0x1C,0x20,0x1E};
static	const	signed char	Duties[4][16] = {{+4,+4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4},{+4,+4,+4,+4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4},{+4,+4,+4,+4,+4,+4,+4,+4,-4,-4,-4,-4,-4,-4,-4,-4},{+4,+4,+4,+4,+4,+4,+4,+4,+4,+4,+4,+4,-4,-4,-4,-4}};

static	void	MMC5Square_CheckActive (PMMC5sqr Chan)
{
	if (Chan->ValidFreq = (Chan->freq >= 0x8))
		Chan->Active = Chan->Enabled && Chan->Timer;
	else	Chan->Active = FALSE;
}
static	void	MMC5Square_Write (PMMC5sqr Chan, int Reg, int Val)
{
	switch (Reg)
	{
	case 0:	Chan->byte0 = Val;
		Chan->EnvCtr = Chan->volume + 1;
		Chan->Vol = Chan->envelope ? Chan->volume : Chan->Envelope;
		break;
	case 2:	Chan->byte2 = Val;
		break;
	case 3:	Chan->byte3 = Val;
		if (Chan->Enabled)
			Chan->Timer = LengthCounts[Chan->timer];
		Chan->Envelope = 0xF;
		Chan->CurD = 0;
		break;
	case 4:	if (!(Chan->Enabled = Val ? TRUE : FALSE))
			Chan->Timer = 0;
		break;
	}
	MMC5Square_CheckActive(Chan);
}
static	int	MMC5_GenerateSquare (PMMC5sqr Chan, int Cycles)
{
	if (!Chan->Active)
		return 0;
	Chan->Cycles -= Cycles;
	while (Chan->Cycles <= 0)
	{
		Chan->Cycles += Chan->freq + 1;
		Chan->CurD = (Chan->CurD + 1) & 0xF;
		Chan->Pos = Duties[Chan->duty][Chan->CurD] * Chan->Vol;
	}
	if (!Chan->envelope)
	{
		Chan->EnvCtr -= Cycles;
		while (Chan->EnvCtr <= 0)
		{
			Chan->EnvCtr += (Chan->volume + 1) * QUARTER_FRAME_LEN;
			if (Chan->Envelope)
				Chan->Envelope--;
			else	Chan->Envelope = Chan->wavehold ? 0xF : 0x0;
			Chan->Vol = Chan->envelope ? Chan->volume : Chan->Envelope;
		}
	}
	if (!Chan->wavehold)
		Chan->Timer -= Cycles;
	MMC5Square_CheckActive(Chan);
	return Chan->Pos;
}

void	MMC5sound_Init (void)
{
	memset(&MMC5sound,0,sizeof(TMMC5sound));
}

void	MMC5sound_Write (int Where, int What)
{
	switch (Where)
	{
	case 0x5000:	MMC5Square_Write(&MMC5sound.Sqr0,0,What);	break;
	case 0x5002:	MMC5Square_Write(&MMC5sound.Sqr0,2,What);	break;
	case 0x5003:	MMC5Square_Write(&MMC5sound.Sqr0,3,What);	break;
	case 0x5004:	MMC5Square_Write(&MMC5sound.Sqr1,0,What);	break;
	case 0x5006:	MMC5Square_Write(&MMC5sound.Sqr1,2,What);	break;
	case 0x5007:	MMC5Square_Write(&MMC5sound.Sqr1,3,What);	break;
	case 0x5010:	if (What & 1)
				EMU->DbgOut("Ack! MMC5 RAW PCM is being used!");
			break;
	case 0x5011:	MMC5sound.PCM = What;
			break;
	case 0x5015:	MMC5Square_Write(&MMC5sound.Sqr0,4,What & 0x01);
			MMC5Square_Write(&MMC5sound.Sqr1,4,What & 0x02);
			break;
	}
}

int	_MAPINT	MMC5sound_Get (int Cycles)
{
	int z = 0;
	if (MMC5sound.Sqr0.Enabled)	z += MMC5_GenerateSquare(&MMC5sound.Sqr0,Cycles);
	if (MMC5sound.Sqr1.Enabled)	z += MMC5_GenerateSquare(&MMC5sound.Sqr1,Cycles);
	z += MMC5sound.PCM;
	return z << 5;
}

int	_MAPINT	MMC5sound_SaveLoad (int mode, int x, char *data)
{
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.byte0)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.byte2)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.byte3)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.Vol)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.Envelope)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr0.Enabled)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr0.Timer)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr0.EnvCtr)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr0.Cycles)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.byte0)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.byte2)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.byte3)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.Vol)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.Envelope)
	SAVELOAD_BYTE(mode,x,data,MMC5sound.Sqr1.Enabled)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr1.Timer)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr1.EnvCtr)
	SAVELOAD_LONG(mode,x,data,MMC5sound.Sqr1.Cycles)
	return x;
}

void	MMC5sound_Destroy (void)
{
}