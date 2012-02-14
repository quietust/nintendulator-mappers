/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#pragma once

#include	"..\interface.h"

namespace MMC6
{
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SyncMirror	(void);
int		GetPRGBank	(int);
int		GetCHRBank	(int);
void		SyncPRG		(int,int);
void		SyncCHR_ROM	(int,int);
void		SyncCHR_RAM	(int,int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
int	MAPINT	CPURead7	(int,int);
void	MAPINT	CPUWrite7	(int,int,int);
void	MAPINT	CPUWrite89	(int,int,int);
void	MAPINT	CPUWriteAB	(int,int,int);
void	MAPINT	CPUWriteCD	(int,int,int);
void	MAPINT	CPUWriteEF	(int,int,int);
void	MAPINT	PPUCycle	(int,int,int,int);
} // namespace MMC6
