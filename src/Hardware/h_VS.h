/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

#include	"..\interface.h"

namespace VS
{
void			Load		(void);
void			Reset		(RESET_TYPE);
void			Unload		(void);
int		MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
int		MAPINT	Read4		(int,int);
int		MAPINT	Read67		(int,int);
void		MAPINT	Write67		(int,int,int);
void		MAPINT	CPUCycle	(void);
unsigned char	MAPINT	Config		(CFG_TYPE,unsigned char);
} // namespace VS
