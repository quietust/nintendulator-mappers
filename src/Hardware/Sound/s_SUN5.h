/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	S_SUN5_H
#define	S_SUN5_H

namespace SUN5sound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		Write		(int,int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
} // namespace SUN5sound

#endif	// S_SUN5_H