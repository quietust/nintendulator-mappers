#ifndef	S_VRC6_H
#define	S_VRC6_H

void		VRC6sound_Load		(void);
void		VRC6sound_Reset		(RESET_TYPE);
void		VRC6sound_Unload	(void);
void		VRC6sound_Write		(int,int);
int	_MAPINT	VRC6sound_Get		(int);
int	_MAPINT	VRC6sound_SaveLoad	(STATE_TYPE,int,unsigned char *);

#endif	/* S_VRC6_H */
