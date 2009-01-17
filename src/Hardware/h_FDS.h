/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_FDS_H
#define	H_FDS_H

#include	"..\interface.h"

void			FDS_Load	(void);
void			FDS_Reset	(RESET_TYPE);
void			FDS_Unload	(void);
int		MAPINT	FDS_SaveLoad	(STATE_TYPE,int,unsigned char *);
void		MAPINT	FDS_CPUCycle	(void);
unsigned char	MAPINT	FDS_Config	(CFG_TYPE,unsigned char);
int		MAPINT	FDS_MapperSnd	(int);
int		MAPINT	FDS_Read	(int,int);
void		MAPINT	FDS_Write	(int,int,int);

#endif	/* H_FDS_H */