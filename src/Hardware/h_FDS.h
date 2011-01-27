/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_FDS_H
#define	H_FDS_H

#include	"..\interface.h"

namespace FDS
{
BOOL			Load		(void);
void			Reset		(RESET_TYPE);
void			Unload		(void);
int		MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void		MAPINT	CPUCycle	(void);
unsigned char	MAPINT	Config		(CFG_TYPE,unsigned char);
int		MAPINT	MapperSnd	(int);
int		MAPINT	Read		(int,int);
void		MAPINT	Write		(int,int,int);
} // namespace FDS

#endif	// H_FDS_H