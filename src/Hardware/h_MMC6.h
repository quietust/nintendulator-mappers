#ifndef	H_MMC6_H
#define	H_MMC6_H

#include	"..\interface.h"

typedef	struct	MMC6
{
	u8 IRQenabled, IRQcounter, IRQlatch, IRQreload;
	u8 IRQaddr;
	u8 Cmd;
	u8 PRG[4];
	u8 CHR[8];
	u8 WRAMEnab;
	u8 Mirror;
	FCPURead CPURead7;
	FCPUWrite CPUWrite7;
	FSync Sync;
}	TMMC6, *PMMC6;

void		MMC6_Init	(RESET_TYPE,FSync);
void		MMC6_Destroy	(void);
void		MMC6_SyncMirror	(void);
int		MMC6_GetPRGBank	(int);
int		MMC6_GetCHRBank	(int);
void		MMC6_SyncPRG	(int,int);
void		MMC6_SyncCHR_ROM(int,int);
void		MMC6_SyncCHR_RAM(int,int);
int	_MAPINT	MMC6_SaveLoad	(STATE_TYPE,int,unsigned char *);
int	_MAPINT	MMC6_CPURead7	(int,int);
void	_MAPINT	MMC6_CPUWrite7	(int,int,int);
void	_MAPINT	MMC6_CPUWrite89	(int,int,int);
void	_MAPINT	MMC6_CPUWriteAB	(int,int,int);
void	_MAPINT	MMC6_CPUWriteCD	(int,int,int);
void	_MAPINT	MMC6_CPUWriteEF	(int,int,int);
void	_MAPINT	MMC6_PPUCycle	(int,int,int,int);

#endif	/* H_MMC6_H */