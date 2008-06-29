/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	S_FDS_H
#define	S_FDS_H

void		FDSsound_Load		(void);
void		FDSsound_Reset		(RESET_TYPE);
void		FDSsound_Unload		(void);
int		FDSsound_Read		(int);
void		FDSsound_Write		(int,int);
int	MAPINT	FDSsound_Get		(int);
int	MAPINT	FDSsound_SaveLoad	(STATE_TYPE,int,unsigned char *);

#endif	/* S_FDS_H */