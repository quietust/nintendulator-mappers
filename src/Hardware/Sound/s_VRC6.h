#ifndef	S_VRC6_H
#define	S_VRC6_H

void	VRC6sound_Init (void);
void	VRC6sound_Write (int,int);
int	_MAPINT	VRC6sound_Get (int);
int	_MAPINT	VRC6sound_SaveLoad (int,int,char *);
void	VRC6sound_Destroy (void);

#endif	/* S_VRC6_H */
