#ifndef	S_VRC7_H
#define	S_VRC7_H

void	VRC7sound_Init (void);
void	VRC7sound_Write (int,int);
int	_MAPINT	VRC7sound_Get (int);
int	_MAPINT	VRC7sound_SaveLoad (int,int,char *);
void	VRC7sound_Destroy (void);

#endif	/* S_VRC7_H */