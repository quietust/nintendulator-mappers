#ifndef H_LATCH_H
#define	H_LATCH_H

#include	"..\interface.h"

#define	BUS_CONFLICTS	/* Enable bus conflicts */

typedef	struct	Latch
{
	u8 Data;
	u16_n Addr;
	FSync Sync;
	int BusConflicts;
}	TLatch, PLatch;

extern	TLatch	Latch;

void		Latch_Load		(FSync,BOOL);
void		Latch_Reset		(RESET_TYPE);
void		Latch_Unload		(void);
int	_MAPINT	Latch_SaveLoad_AD	(STATE_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_AL	(STATE_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_A	(STATE_TYPE,int,unsigned char *);
int	_MAPINT	Latch_SaveLoad_D	(STATE_TYPE,int,unsigned char *);
void	_MAPINT	Latch_Write		(int,int,int);

#endif	/* H_LATCH_H */