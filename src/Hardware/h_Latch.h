/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef H_LATCH_H
#define	H_LATCH_H

#include	"..\interface.h"

#define	BUS_CONFLICTS	/* Enable bus conflicts */

namespace Latch
{
extern uint8 Data;
extern uint16_n Addr;

void		Load		(FSync,BOOL);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int	MAPINT	SaveLoad_AD	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_AL	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_A	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_D	(STATE_TYPE,int,unsigned char *);
void	MAPINT	Write		(int,int,int);
} // namespace Latch

#endif	// H_LATCH_H