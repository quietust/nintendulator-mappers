/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_FME7_H
#define	H_FME7_H

#include	"..\interface.h"

namespace FME7
{
void		Load		(FSync);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SyncMirror	(void);
void		SyncPRG		(int,int);
void		SyncCHR		(int,int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	Write89		(int,int,int);
void	MAPINT	WriteAB		(int,int,int);
void	MAPINT	WriteCDEF	(int,int,int);
void	MAPINT	CPUCycle	(void);
int	MAPINT	GenSound	(int);
} // namespace FME7

#endif	// H_FME7_H
