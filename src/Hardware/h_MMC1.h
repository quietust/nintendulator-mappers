#ifndef	H_MMC1_H
#define	H_MMC1_H

#include	"..\interface.h"

typedef	struct	MMC1
{
	u8 Latch, LatchPos;
	u8 Regs[4];
	FSync Sync;
}	TMMC1, *PMMC1;

extern	TMMC1	MMC1;

void		MMC1_Load	(FSync);
void		MMC1_Reset	(RESET_TYPE);
void		MMC1_Unload	(void);
int	_MAPINT	MMC1_SaveLoad		(STATE_TYPE,int,unsigned char *);
void	_MAPINT	MMC1_Write		(int,int,int);

void		MMC1_SyncMirror		(void);
int		MMC1_GetPRGBankLo	(void);
int		MMC1_GetPRGBankHi	(void);
int		MMC1_GetCHRBankLo	(void);
int		MMC1_GetCHRBankHi	(void);
BOOL		MMC1_GetWRAMEnabled	(void);

void		MMC1_SyncPRG		(int,int);
void		MMC1_SyncCHR_ROM	(int,int);
void		MMC1_SyncCHR_RAM	(int,int);
void		MMC1_SyncWRAM		(void);

#endif	/* H_MMC1_H */