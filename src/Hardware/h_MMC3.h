/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC3_H
#define	H_MMC3_H

#include	"..\interface.h"

namespace MMC3
{
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SyncMirror	(void);
int		GetPRGBank	(int);
int		GetCHRBank	(int);
void		SyncPRG		(int,int);
void		SyncWRAM	(void);
void		SyncCHR_ROM	(int,int);
void		SyncCHR_RAM	(int,int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	CPUWrite67	(int,int,int);
void	MAPINT	CPUWrite89	(int,int,int);
void	MAPINT	CPUWriteAB	(int,int,int);
void	MAPINT	CPUWriteCD	(int,int,int);
void	MAPINT	CPUWriteEF	(int,int,int);
void	MAPINT	PPUCycle	(int,int,int,int);
} // namespace MMC3

#endif	// H_MMC3_H