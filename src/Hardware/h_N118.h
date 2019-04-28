/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

#include	"..\interface.h"

namespace N118
{
extern uint8_t CHR[6];
	
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SyncPRG		(void);
void		SyncCHR		(void);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	CPUWrite89	(int,int,int);
} // namespace N118
