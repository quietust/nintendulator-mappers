/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC3_H
#define	H_MMC3_H

#include	"..\interface.h"

void		MMC3_Load	(FSync);
void		MMC3_Reset	(RESET_TYPE);
void		MMC3_Unload	(void);
void		MMC3_SyncMirror	(void);
int		MMC3_GetPRGBank	(int);
int		MMC3_GetCHRBank	(int);
void		MMC3_SyncPRG	(int,int);
void		MMC3_SyncWRAM	(void);
void		MMC3_SyncCHR_ROM(int,int);
void		MMC3_SyncCHR_RAM(int,int);
int	MAPINT	MMC3_SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	MMC3_CPUWrite67	(int,int,int);
void	MAPINT	MMC3_CPUWrite89	(int,int,int);
void	MAPINT	MMC3_CPUWriteAB	(int,int,int);
void	MAPINT	MMC3_CPUWriteCD	(int,int,int);
void	MAPINT	MMC3_CPUWriteEF	(int,int,int);
void	MAPINT	MMC3_PPUCycle	(int,int,int,int);

#endif	/* H_MMC3_H */