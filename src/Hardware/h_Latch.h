#ifndef __LATCH_H__
#define	__LATCH_H__

#include	"..\interface.h"

#define	BUS_CONFLICTS	/* Enable bus conflicts */

typedef	struct	Latch
{
	u8 Data;
	u16_n Addr;
	void	(*Sync)	(void);
	int BusConflicts;
}	TLatch, PLatch;

extern	TLatch	Latch;

void		Latch_Init		(void (*)(void),BOOL,BOOL);
void		Latch_Destroy		(void);
int	_MAPINT	Latch_SaveLoad_AD	(int,int,char *);
int	_MAPINT	Latch_SaveLoad_AL	(int,int,char *);
int	_MAPINT	Latch_SaveLoad_A	(int,int,char *);
int	_MAPINT	Latch_SaveLoad_D	(int,int,char *);
void	_MAPINT	Latch_Write		(int,int,int);

#endif	/* __LATCH_H__ */