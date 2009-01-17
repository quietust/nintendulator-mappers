/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_VS_H
#define	H_VS_H

#include	"..\interface.h"

void			VS_Load		(void);
void			VS_Reset	(RESET_TYPE);
void			VS_Unload	(void);
int		MAPINT	VS_SaveLoad	(STATE_TYPE,int,unsigned char *);
int		MAPINT	VS_Read		(int,int);
void		MAPINT	VS_CPUCycle	(void);
unsigned char	MAPINT VS_Config	(CFG_TYPE,unsigned char);

#endif	/* H_VS_H */