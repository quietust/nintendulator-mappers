#include	"..\..\interface.h"
#include	"s_MMC5.h"

// Nintendo MMC5, based on the NES APU

typedef	struct	MMC5Sqr
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
}	TMMC5sqr, *PMMC5sqr;

typedef	struct	MMC5sound
{
	TMMC5sqr Sqr0, Sqr1;
	unsigned char byte0, byte2, byte3, byte4, byte6, byte7, byte10, byte11, byte15;
	int PCM;
}	TMMC5sound, *PMMC5sound;

static	TMMC5sound	MMC5sound;

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

static	void	MMC5Square_CheckActive (PMMC5sqr Chan)
{
	if ((Chan->ValidFreq = (Chan->freq >= 0x8)) && (Chan->Timer))
	{
		Chan->Active = TRUE;
		Chan->Pos = Duties[Chan->duty][Chan->CurD] * Chan->Vol;
	}
	else
	{
		Chan->Active = FALSE;
		Chan->Pos = 0;
	}
}
static	void	MMC5Square_Write (PMMC5sqr Chan, int Reg, int Val)
{
	switch (Reg)
	{
	case 0:	Chan->volume = Val & 0xF;
		Chan->envelope = Val & 0x10;
		Chan->wavehold = Val & 0x20;
		Chan->duty = (Val >> 6) & 0x3;
		Chan->Vol = Chan->envelope ? Chan->volume : Chan->Envelope;
		break;
	case 2:	Chan->freq &= 0x700;
		Chan->freq |= Val;
		break;
	case 3:	Chan->freq &= 0xFF;
		Chan->freq |= (Val & 0x7) << 8;
		if (Chan->Enabled)
			Chan->Timer = LengthCounts[(Val >> 3) & 0x1F] << 1;
		Chan->CurD = 0;
		Chan->EnvClk = TRUE;
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
		Chan->Cycles += (Chan->freq + 1) << 1;
		Chan->CurD = (Chan->CurD + 1) & 0x7;
		if (Chan->Active)
			Chan->Pos = Duties[Chan->duty][Chan->CurD] * Chan->Vol;
	}

	Chan->FrameCycles -= Cycles;
	while (Chan->FrameCycles <= 0)
	{
		Chan->FrameCycles += QUARTER_FRAME_LEN;
		if (Chan->EnvClk)
		{
			Chan->EnvClk = FALSE;
			Chan->Envelope = 0xF;
			Chan->EnvCtr = Chan->volume + 1;
		}
		else if (!--Chan->EnvCtr)
		{
			Chan->EnvCtr = Chan->volume + 1;
			if (Chan->Envelope)
				Chan->Envelope--;
			else	Chan->Envelope = Chan->wavehold ? 0xF : 0x0;
		}

		if (Chan->Timer && !Chan->wavehold)
			Chan->Timer--;
	}
	Chan->Vol = Chan->envelope ? Chan->volume : Chan->Envelope;
	MMC5Square_CheckActive(Chan);
	return Chan->Pos;
}

void	MMC5sound_Init (void)
{
	memset(&MMC5sound,0,sizeof(TMMC5sound));
}

void	MMC5sound_Write (int Addr, int Val)
{
	switch (Addr)
	{
	case 0x5000:	MMC5Square_Write(&MMC5sound.Sqr0,0,MMC5sound.byte0 = Val);	break;
	case 0x5002:	MMC5Square_Write(&MMC5sound.Sqr0,2,MMC5sound.byte2 = Val);	break;
	case 0x5003:	MMC5Square_Write(&MMC5sound.Sqr0,3,MMC5sound.byte3 = Val);	break;
	case 0x5004:	MMC5Square_Write(&MMC5sound.Sqr1,0,MMC5sound.byte4 = Val);	break;
	case 0x5006:	MMC5Square_Write(&MMC5sound.Sqr1,2,MMC5sound.byte6 = Val);	break;
	case 0x5007:	MMC5Square_Write(&MMC5sound.Sqr1,3,MMC5sound.byte7 = Val);	break;
	case 0x5010:	MMC5sound.byte10 = Val;
			if (Val & 1)
				EMU->DbgOut("Ack! MMC5 RAW PCM is being used!");
			break;
	case 0x5011:	MMC5sound.PCM = MMC5sound.byte11 = Val;
			break;
	case 0x5015:	MMC5sound.byte15 = Val;
			MMC5Square_Write(&MMC5sound.Sqr0,4,Val & 0x01);
			MMC5Square_Write(&MMC5sound.Sqr1,4,Val & 0x02);
			break;
	}
}

int	_MAPINT	MMC5sound_Get (int Cycles)
{
	int z = 0;
	if (MMC5sound.Sqr0.Enabled)	z += MMC5_GenerateSquare(&MMC5sound.Sqr0,Cycles);
	if (MMC5sound.Sqr1.Enabled)	z += MMC5_GenerateSquare(&MMC5sound.Sqr1,Cycles);
	z += MMC5sound.PCM;
	return z << 6;
}

int	_MAPINT	MMC5sound_SaveLoad (STATE_TYPE mode, int x, unsigned char *data)
{
	if (mode == STATE_SAVE)
		memcpy(data,&MMC5sound,sizeof(MMC5sound));
	else if (mode == STATE_LOAD)
		memcpy(&MMC5sound,data,sizeof(MMC5sound));
	else if (mode != STATE_SIZE)
		MessageBox(hWnd,"Invalid save/load type!",__FILE__,MB_OK);
	x += sizeof(MMC5sound);
	return x;
}

void	MMC5sound_Destroy (void)
{
}