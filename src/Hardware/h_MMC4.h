#ifndef	H_MMC4_H
#define	H_MMC4_H

#include	"..\interface.h"

typedef	struct	MMC4
{
	u8 PRG;
	u8 LatchState[2];
	u8 Latch0[2];
	u8 Latch1[2];
	u8 Mirror;
	void	(*Sync)	(void);
	FPPURead PPURead3, PPURead7;
}	TMMC4, *PMMC4;

void		MMC4_Init	(void (*)(void));
void		MMC4_Destroy	(void);
int	_MAPINT	MMC4_SaveLoad	(int,int,char *);
void		MMC4_SyncPRG	(int,int);
void		MMC4_SyncCHR	(void);
void		MMC4_SyncMirror	(void);
void	_MAPINT	MMC4_CPUWriteA	(int,int,int);
void	_MAPINT	MMC4_CPUWriteB	(int,int,int);
void	_MAPINT	MMC4_CPUWriteC	(int,int,int);
void	_MAPINT	MMC4_CPUWriteD	(int,int,int);
void	_MAPINT	MMC4_CPUWriteE	(int,int,int);
void	_MAPINT	MMC4_CPUWriteF	(int,int,int);
int	_MAPINT	MMC4_PPURead3	(int,int);
int	_MAPINT	MMC4_PPURead7	(int,int);

#endif	/* H_MMC4_H */