/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC4_H
#define	H_MMC4_H

#include	"..\interface.h"

void		MMC4_Load	(FSync);
void		MMC4_Reset	(RESET_TYPE);
void		MMC4_Unload	(void);
int	MAPINT	MMC4_SaveLoad	(STATE_TYPE,int,unsigned char *);
void		MMC4_SyncPRG	(int,int);
void		MMC4_SyncCHR	(void);
void		MMC4_SyncMirror	(void);
void	MAPINT	MMC4_CPUWriteA	(int,int,int);
void	MAPINT	MMC4_CPUWriteB	(int,int,int);
void	MAPINT	MMC4_CPUWriteC	(int,int,int);
void	MAPINT	MMC4_CPUWriteD	(int,int,int);
void	MAPINT	MMC4_CPUWriteE	(int,int,int);
void	MAPINT	MMC4_CPUWriteF	(int,int,int);
int	MAPINT	MMC4_PPURead3	(int,int);
int	MAPINT	MMC4_PPURead7	(int,int);

#endif	/* H_MMC4_H */