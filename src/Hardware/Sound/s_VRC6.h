/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	S_VRC6_H
#define	S_VRC6_H

namespace VRC6sound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		Write		(int,int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
} // namespace VRC6sound

#endif	// S_VRC6_H
