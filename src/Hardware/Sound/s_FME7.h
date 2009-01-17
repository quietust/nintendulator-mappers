/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	S_FME7_H
#define	S_FME7_H

void		FME7sound_Load		(void);
void		FME7sound_Reset		(RESET_TYPE);
void		FME7sound_Unload	(void);
void		FME7sound_Write		(int,int);
int	MAPINT	FME7sound_Get		(int);
int	MAPINT	FME7sound_SaveLoad	(STATE_TYPE,int,unsigned char *);

#endif	/* S_FME7_H */