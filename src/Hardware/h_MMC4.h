/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#pragma once

#include	"..\interface.h"

namespace MMC4
{
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void		SyncPRG		(int,int);
void		SyncCHR		(void);
void		SyncMirror	(void);
void	MAPINT	CPUWriteA	(int,int,int);
void	MAPINT	CPUWriteB	(int,int,int);
void	MAPINT	CPUWriteC	(int,int,int);
void	MAPINT	CPUWriteD	(int,int,int);
void	MAPINT	CPUWriteE	(int,int,int);
void	MAPINT	CPUWriteF	(int,int,int);
int	MAPINT	PPURead3	(int,int);
int	MAPINT	PPURead7	(int,int);
} // namespace MMC4
