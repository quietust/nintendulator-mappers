#ifndef H_LATCH_H
#define	H_LATCH_H

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

void		Latch_Init		(RESET_TYPE,void (*)(void),BOOL);
void		Latch_Destroy		(void);
int	_MAPINT	Latch_SaveLoad_AD	(SAVELOAD_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_AL	(SAVELOAD_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_A	(SAVELOAD_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_D	(SAVELOAD_TYPE,int,unsigned char *);
void	_MAPINT	Latch_Write		(int,int,int);

#endif	/* H_LATCH_H */