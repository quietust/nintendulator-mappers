#ifndef	__FME07_H__
#define	__FME07_H__

#include	"..\interface.h"

typedef	struct	FME07
{
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 Cmd;
	u8 PRG[4], CHR[8];
	u8 Mirror;
	void	(*Sync)	(void);
}	TFME07, *PFME07;

void		FME07_Init		(void (*)(void));
void		FME07_Destroy		(void);
void		FME07_SyncMirror	(void);
void		FME07_SyncPRG		(int,int);
void		FME07_SyncCHR		(int,int);
int	_MAPINT	FME07_SaveLoad		(int,int,char *);
void	_MAPINT	FME07_Write89		(int,int,int);
void	_MAPINT	FME07_WriteAB		(int,int,int);
void	_MAPINT	FME07_WriteCDEF		(int,int,int);
void	_MAPINT	FME07_CPUCycle		(void);
int	_MAPINT	FME07_GenSound		(int);

#endif	/* __FME07_H__ */
