#ifndef	__FME7_H__
#define	__FME7_H__

#include	"..\interface.h"

typedef	struct	FME7
{
	u8 IRQenabled;
	u16_n IRQcounter;
	u8 Cmd;
	u8 PRG[4], CHR[8];
	u8 Mirror;
	void	(*Sync)	(void);
}	TFME7, *PFME7;

void		FME7_Init		(void (*)(void));
void		FME7_Destroy		(void);
void		FME7_SyncMirror	(void);
void		FME7_SyncPRG		(int,int);
void		FME7_SyncCHR		(int,int);
int	_MAPINT	FME7_SaveLoad		(int,int,char *);
void	_MAPINT	FME7_Write89		(int,int,int);
void	_MAPINT	FME7_WriteAB		(int,int,int);
void	_MAPINT	FME7_WriteCDEF		(int,int,int);
void	_MAPINT	FME7_CPUCycle		(void);
int	_MAPINT	FME7_GenSound		(int);

#endif	/* __FME7_H__ */
