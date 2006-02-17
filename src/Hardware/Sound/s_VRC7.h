#ifndef	S_VRC7_H
#define	S_VRC7_H

void		VRC7sound_Load		(void);
void		VRC7sound_Reset		(RESET_TYPE);
void		VRC7sound_Unload	(void);
void		VRC7sound_Write		(int,int);
int	MAPINT	VRC7sound_Get		(int);
int	MAPINT	VRC7sound_SaveLoad	(STATE_TYPE,int,unsigned char *);

#endif	/* S_VRC7_H */