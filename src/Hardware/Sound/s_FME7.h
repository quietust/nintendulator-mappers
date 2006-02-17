#ifndef	S_FME7_H
#define	S_FME7_H

void	FME7sound_Init (void);
void	FME7sound_Write (int,int);
int	_MAPINT	FME7sound_Get (int);
int	_MAPINT	FME7sound_SaveLoad (int,int,char *);
void	FME7sound_Destroy (void);

#endif	/* S_FME7_H */