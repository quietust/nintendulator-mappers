/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC2_H
#define	H_MMC2_H

#include	"..\interface.h"

void		MMC2_Load	(FSync);
void		MMC2_Reset	(RESET_TYPE);
void		MMC2_Unload	(void);
int	MAPINT	MMC2_SaveLoad	(STATE_TYPE,int,unsigned char *);
void		MMC2_SyncPRG	(int,int);
void		MMC2_SyncCHR	(void);
void		MMC2_SyncMirror	(void);
void	MAPINT	MMC2_CPUWriteA	(int,int,int);
void	MAPINT	MMC2_CPUWriteB	(int,int,int);
void	MAPINT	MMC2_CPUWriteC	(int,int,int);
void	MAPINT	MMC2_CPUWriteD	(int,int,int);
void	MAPINT	MMC2_CPUWriteE	(int,int,int);
void	MAPINT	MMC2_CPUWriteF	(int,int,int);
int	MAPINT	MMC2_PPURead3	(int,int);
int	MAPINT	MMC2_PPURead7	(int,int);

#endif	/* H_MMC2_H */