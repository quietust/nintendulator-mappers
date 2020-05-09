/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

#include	"..\interface.h"

#define	BUS_CONFLICTS	/* Enable bus conflicts */

namespace Latch
{
extern uint8_t Data;
extern uint16_n Addr;

void		Load		(FSync,BOOL,BOOL);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int	MAPINT	SaveLoad_AD	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_AL	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_A	(STATE_TYPE,int,unsigned char *);
int	MAPINT	SaveLoad_D	(STATE_TYPE,int,unsigned char *);
void	MAPINT	Write		(int,int,int);
} // namespace Latch
