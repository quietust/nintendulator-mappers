/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_N118_H
#define	H_N118_H

#include	"..\interface.h"

namespace N118
{
extern uint8 CHR[6];
	
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SyncPRG		(void);
void		SyncCHR		(void);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	CPUWrite89	(int,int,int);
} // namespace N118

#endif	// H_N118_H