/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_FME7_H
#define	H_FME7_H

#include	"..\interface.h"

void		FME7_Load	(FSync);
void		FME7_Reset	(RESET_TYPE);
void		FME7_Unload	(void);
void		FME7_SyncMirror	(void);
void		FME7_SyncPRG	(int,int);
void		FME7_SyncCHR	(int,int);
int	MAPINT	FME7_SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	FME7_Write89	(int,int,int);
void	MAPINT	FME7_WriteAB	(int,int,int);
void	MAPINT	FME7_WriteCDEF	(int,int,int);
void	MAPINT	FME7_CPUCycle	(void);
int	MAPINT	FME7_GenSound	(int);

#endif	/* H_FME7_H */
