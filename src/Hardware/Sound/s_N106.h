/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	S_N106_H
#define	S_N106_H

namespace N106sound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int		Read		(int);
void		Write		(int,int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
} // namespace N106sound

#endif	// S_N106_H