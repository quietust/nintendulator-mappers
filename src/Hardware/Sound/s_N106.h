#ifndef	S_N106_H
#define	S_N106_H

void		N106sound_Load		(void);
void		N106sound_Reset		(RESET_TYPE);
void		N106sound_Unload	(void);
int		N106sound_Read		(int);
void		N106sound_Write		(int,int);
int	MAPINT	N106sound_Get		(int);
int	MAPINT	N106sound_SaveLoad	(STATE_TYPE,int,unsigned char *);

#endif	/* S_N106_H */