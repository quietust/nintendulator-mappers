#ifndef	S_N106_H
#define	S_N106_H

void	N106sound_Init (void);
int	N106sound_Read (int);
void	N106sound_Write (int,int);
int	_MAPINT	N106sound_Get (int);
int	_MAPINT	N106sound_SaveLoad (SAVELOAD_TYPE,int,unsigned char *);
void	N106sound_Destroy (void);

#endif	/* S_N106_H */