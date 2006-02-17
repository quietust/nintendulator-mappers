#ifndef	__MMC3_H__
#define	__MMC3_H__

#include	"..\interface.h"

typedef	struct	MMC3
{
	u8 IRQenabled, IRQcounter, IRQlatch;
	u8 Cmd;
	u8 PRG[4];
	u8 CHR[8];
	u8 WRAMEnab;
	u8 Mirror;
	FCPUWrite CPUWrite67;
	void	(*Sync)	(void);
}	TMMC3, *PMMC3;

void		MMC3_Init	(void (*)(void));
void		MMC3_Destroy	(void);
void		MMC3_SyncMirror	(void);
int		MMC3_GetPRGBank	(int);
int		MMC3_GetCHRBank	(int);
void		MMC3_SyncPRG	(int,int);
void		MMC3_SyncWRAM	(void);
void		MMC3_SyncCHR_ROM(int,int);
void		MMC3_SyncCHR_RAM(int,int);
int	_MAPINT	MMC3_SaveLoad	(int,int,char *);
void	_MAPINT	MMC3_CPUWrite67	(int,int,int);
void	_MAPINT	MMC3_CPUWrite89	(int,int,int);
void	_MAPINT	MMC3_CPUWriteAB	(int,int,int);
void	_MAPINT	MMC3_CPUWriteCD	(int,int,int);
void	_MAPINT	MMC3_CPUWriteEF	(int,int,int);
void	_MAPINT	MMC3_PPUCycle	(int,int,int,int);

#endif	/* __MMC3_H__ */