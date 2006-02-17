#ifndef	__MMC6_H__
#define	__MMC6_H__

#include	"..\interface.h"

typedef	struct	MMC6
{
	u8 IRQenabled, IRQcounter, IRQlatch;
	u8 Cmd;
	u8 PRG[2];
	u8 CHR[8];
	u8 WriteWRAM;
	u8 Mirror;
	FCPURead CPURead67;
	FCPUWrite CPUWrite67;
	void	(*Sync)	(void);
}	TMMC6, *PMMC6;

void		MMC6_Init	(void (*)(void));
void		MMC6_Destroy	(void);
void		MMC6_SyncMirror	(void);
int		MMC6_GetPRGBank	(int);
int		MMC6_GetCHRBank	(int);
void		MMC6_SyncPRG	(int,int);
void		MMC6_SyncCHR_ROM(int,int);
void		MMC6_SyncCHR_RAM(int,int);
int	_MAPINT	MMC6_SaveLoad	(int,int,char *);
int	_MAPINT	MMC6_CPURead67	(int,int);
void	_MAPINT	MMC6_CPUWrite67	(int,int,int);
void	_MAPINT	MMC6_CPUWrite89	(int,int,int);
void	_MAPINT	MMC6_CPUWriteAB	(int,int,int);
void	_MAPINT	MMC6_CPUWriteCD	(int,int,int);
void	_MAPINT	MMC6_CPUWriteEF	(int,int,int);
void	_MAPINT	MMC6_PPUCycle	(int,int,int,int);

#endif	/* __MMC6_H__ */