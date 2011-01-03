/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_VS_H
#define	H_VS_H

#include	"..\interface.h"

namespace VS
{
void			Load		(void);
void			Reset		(RESET_TYPE);
void			Unload		(void);
int		MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
int		MAPINT	Read		(int,int);
void		MAPINT	CPUCycle	(void);
unsigned char	MAPINT	Config		(CFG_TYPE,unsigned char);
} // namespace VS

#endif	// H_VS_H